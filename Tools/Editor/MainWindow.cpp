#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <QBoxLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.hpp"
#include "SceneWidget.hpp"
#include "WindowMenu.hpp"
#include "CreateCameraCommand.hpp"
#include "CreateGoCommand.hpp"
#include "TransformInspector.hpp"

MainWindow::MainWindow()
{
    sceneTree = new QTreeWidget();
    sceneTree->setColumnCount( 1 );
    // TODO: Change itemClicked to something that can handle multi-selection properly.
    connect( sceneTree, &QTreeWidget::itemClicked, [&](QTreeWidgetItem* item, int /* column */) { SelectTreeItem( item ); });
    connect( sceneTree, &QTreeWidget::itemEntered, [&](QTreeWidgetItem* item, int /* column */) { SelectTreeItem( item ); });
    connect( sceneTree, &QTreeWidget::itemChanged, [&](QTreeWidgetItem* item, int /* column */) { HierarchyItemRenamed( item ); });
    sceneTree->setSelectionMode( QAbstractItemView::SelectionMode::ExtendedSelection );

    sceneWidget = new SceneWidget();
    sceneWidget->SetMainWindow( this );
    setWindowTitle( "Editor" );
    connect( sceneWidget, SIGNAL(GameObjectsAddedOrDeleted()), this, SLOT(HandleGameObjectsAddedOrDeleted()) );

    windowMenu.Init( this );
    setMenuBar( windowMenu.menuBar );

    transformInspector.Init( this );
    QBoxLayout* inspectorLayout = new QBoxLayout( QBoxLayout::TopToBottom );
    inspectorLayout->addWidget( transformInspector.GetWidget() );
    inspectorContainer = new QWidget();
    inspectorContainer->setLayout( inspectorLayout );
    inspectorContainer->setMinimumWidth( 380 );

    QSplitter* splitter = new QSplitter();
    splitter->addWidget( sceneTree );
    splitter->addWidget( sceneWidget );
    splitter->addWidget( inspectorContainer );
    setCentralWidget( splitter );

    UpdateHierarchy();
    UpdateInspector();
}

void MainWindow::UpdateInspector()
{
    if (sceneWidget->selectedGameObjects.empty())
    {
        std::cout << "Hiding inspector." << std::endl;
        transformInspector.GetWidget()->hide();
    }
    else
    {
        std::cout << "Showing inspector." << std::endl;
        transformInspector.GetWidget()->show();
    }
}

void MainWindow::ShowAbout()
{
    QMessageBox::about(this, "Controls", "Aether3D Editor by Timo Wiren 2015\n\nControls\nRight mouse and W,S,A,D,Q,E: camera movement\nMiddle mouse: pan");
}

void MainWindow::HandleGameObjectsAddedOrDeleted()
{
    UpdateHierarchy();
}

void MainWindow::HierarchyItemRenamed( QTreeWidgetItem* item )
{
    int renamedIndex = 0;

    // Figures out which game object was renamed.
    for (int i = 0; i < sceneWidget->GetGameObjectCount(); ++i)
    {
        if (sceneTree->topLevelItem( i ) == item)
        {
            renamedIndex = i;
        }
    }

    sceneWidget->GetGameObject( renamedIndex )->SetName( item->text( 0 ).toUtf8().constData() );
    UpdateHierarchy();
}

void MainWindow::SelectTreeItem( QTreeWidgetItem* item )
{
    std::list< ae3d::GameObject* > selectedObjects;
    sceneWidget->selectedGameObjects.clear();

    for (int g = 0; g < sceneWidget->GetGameObjectCount(); ++g)
    {
        for (auto i : sceneTree->selectedItems())
        {
            if (sceneTree->topLevelItem( g ) == i)
            {
                selectedObjects.push_back( sceneWidget->GetGameObject( g ) );
                sceneWidget->selectedGameObjects.push_back( g );
            }
        }
    }

    std::cout << "printing selection: " << std::endl;
    for (auto index : sceneWidget->selectedGameObjects)
    {
        std::cout << "selection: " << index << std::endl;
    }

    UpdateInspector();
    emit GameObjectSelected( selectedObjects );
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    const int macDelete = 16777219;

    if (event->key() == Qt::Key_Escape)
    {
        std::list< ae3d::GameObject* > emptyList;
        emit GameObjectSelected( emptyList );
        sceneWidget->selectedGameObjects.clear();
        sceneTree->clearSelection();
        UpdateInspector();
    }
    else if (event->key() == Qt::Key_Delete || event->key() == macDelete)
    {
        std::cout << "removing selected objects" << std::endl;

        for (int i = 0; i < sceneTree->topLevelItemCount(); ++i)
        {
            if (sceneTree->topLevelItem( i )->isSelected())
            {
                sceneWidget->RemoveGameObject( i );
                sceneWidget->selectedGameObjects.clear();
                std::list< ae3d::GameObject* > emptyList;
                emit GameObjectSelected( emptyList );
                sceneTree->clearSelection();
                UpdateInspector();
            }
        }

        /*for (auto goIndex : sceneWidget->selectedGameObjects)
        {
            std::cout << "removed " << sceneWidget->GetGameObject(goIndex)->GetName() << std::endl;
            sceneWidget->RemoveGameObject( goIndex );
            sceneWidget->selectedGameObjects.clear();
        }*/

        UpdateHierarchy();
    }
}

void MainWindow::CommandCreateCameraComponent()
{
    commandManager.Execute( std::make_shared< CreateCameraCommand >( sceneWidget ) );
}

void MainWindow::CommandCreateGameObject()
{
    commandManager.Execute( std::make_shared< CreateGoCommand >( sceneWidget ) );
    UpdateHierarchy();
}

void MainWindow::LoadScene()
{
    const QString fileName = QFileDialog::getOpenFileName( this, "Open Scene", "", "Scenes (*.scene)" );

    if (fileName != "")
    {
        // TODO: Clear scene.
        // TODO: Ask for confirmation if the scene hasa unsaved modifications.
        sceneWidget->LoadSceneFromFile( fileName.toUtf8().constData() );
    }
}

void MainWindow::SaveScene()
{
#if __APPLE__
    const char* appendDir = "/Documents";
#else
    const char* appendDir = "";
#endif

    QString fileName = QFileDialog::getSaveFileName( this, "Save Scene", QDir::homePath() + appendDir, "Scenes (*.scene)" );
    std::ofstream ofs( fileName.toUtf8().constData() );
    ofs << sceneWidget->GetScene()->GetSerialized();
    const bool saveSuccess = ofs.is_open();

    if (!saveSuccess && fileName.toUtf8() != "")
    {
        QMessageBox::critical( this, "Unable to Save!", "Scene could not be saved." );
    }
}

void MainWindow::UpdateHierarchy()
{
    sceneTree->clear();
    sceneTree->setHeaderLabel( "Hierarchy" );

    QList< QTreeWidgetItem* > nodes;
    const int count = sceneWidget->GetGameObjectCount();

    std::cout << "cleared scene tree" << std::endl;
    for (int i = 0; i < count; ++i)
    {
        nodes.append(new QTreeWidgetItem( (QTreeWidget*)0, QStringList( QString( sceneWidget->GetGameObject( i )->GetName().c_str() ) ) ) );
        nodes.back()->setFlags( nodes.back()->flags() | Qt::ItemIsEditable );
        std::cout << "added to list: " <<  sceneWidget->GetGameObject( i )->GetName()<< std::endl;
    }

    sceneTree->insertTopLevelItems( 0, nodes );

    // Highlights selected game objects.
    for (int i = 0; i < count; ++i)
    {
        for (auto goIndex : sceneWidget->selectedGameObjects)
        {
            if (i == goIndex)
            {
                sceneTree->setCurrentItem( nodes.at(i) );
            }
        }
    }
}
