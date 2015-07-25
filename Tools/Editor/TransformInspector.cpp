#include "TransformInspector.hpp"
#include <QTableWidget>
#include "GameObject.hpp"
#include "TransformComponent.hpp"

using namespace ae3d;

void TransformInspector::Init( QWidget* mainWindow )
{
    table = new QTableWidget( 3, 3 );
    table->setHorizontalHeaderLabels(QString("X;Y;Z").split(";"));
    table->setVerticalHeaderLabels(QString("Position;Rotation;Scale").split(";"));
    table->setSpan( 2, 0, 1, 3 );

    connect(mainWindow, SIGNAL(GameObjectSelected(std::list< ae3d::GameObject* >)),
            this, SLOT(GameObjectSelected(std::list< ae3d::GameObject* >)));
    connect( table, &QTableWidget::itemChanged, [&](QTableWidgetItem * item) { FieldsChanged( item ); });
}

void TransformInspector::GameObjectSelected( std::list< ae3d::GameObject* > gameObjects )
{
    if (gameObjects.empty())
    {
        return;
    }

    ae3d::GameObject* go = gameObjects.front();

    // Position.
    const Vec3& position = go->GetComponent< ae3d::TransformComponent >()->GetLocalPosition();

    char buf[ 50 ];
    sprintf( buf, "%.2f", position.x );
    table->setItem( 0, 0, new QTableWidgetItem( buf ) );

    sprintf( buf, "%.2f", position.y );
    table->setItem( 0, 1, new QTableWidgetItem( buf ) );

    sprintf( buf, "%.2f", position.z );
    table->setItem( 0, 2, new QTableWidgetItem( buf ) );

    // Rotation.
    const Quaternion& rotation = go->GetComponent< ae3d::TransformComponent >()->GetLocalRotation();
    const Vec3 euler = rotation.GetEuler();

    //std::cout << "Rotation quaternion: " << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << std::endl;
    //std::cout << "Rotation euler: " << euler.x << " " << euler.y << " " << euler.z << " " << std::endl;

    sprintf( buf, "%.2f", euler.x );
    table->setItem( 1, 0, new QTableWidgetItem( buf ) );

    sprintf( buf, "%.2f", euler.y );
    table->setItem( 1, 1, new QTableWidgetItem( buf ) );

    sprintf( buf, "%.2f", euler.z );
    table->setItem( 1, 2, new QTableWidgetItem( buf ) );

    // Scale.
    sprintf( buf, "%.2f", 1.0f/*go->GetComponent< ae3d::TransformComponent >()->GetLocalScale()*/ );
    table->setItem( 2, 0, new QTableWidgetItem( buf ) );
}

void TransformInspector::FieldsChanged( QTableWidgetItem* item )
{
    printf("Field changed\n");
    Vec3 position;// = editorState->selectedNode->GetPosition();
    Quaternion rotation;// = editorState->selectedNode->GetRotation();
    Vec3 rotationEuler;// = rotation.GetEuler();

    const std::string newValue = item->text().toUtf8().constData();

    if (item->row() == 0 && item->column() == 0)
    {
        try
        {
            position.x = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            position.x = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 0 && item->column() == 1)
    {
        try
        {
            position.y = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            position.y = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 0 && item->column() == 2)
    {
        try
        {
            position.z = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            position.z = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 1 && item->column() == 0) // Rotation x
    {
        try
        {
            rotationEuler.x = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            rotationEuler.x = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 1 && item->column() == 1) // Rotation y
    {
        try
        {
            rotationEuler.y = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            rotationEuler.y = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 1 && item->column() == 2) // Rotation z
    {
        try
        {
            rotationEuler.z = std::stof( newValue );
        }
        catch (std::invalid_argument& a)
        {
            rotationEuler.z = 0;
            item->setText( "0" );
        }
    }
    else if (item->row() == 2 && item->column() == 0)
    {
        try
        {
            const float scale = std::stof( newValue );
            //editorState->selectedNode->SetScale( scale );
        }
        catch (std::invalid_argument& a)
        {
            //item->setText( std::to_string(editorState->selectedNode->GetScale()).c_str());
        }
    }

    rotation.FromEuler( rotationEuler );

    //std::cout << "Setting rotation from Euler: " << rotationEuler.x << ", " << rotationEuler.y << ", " << rotationEuler.z << std::endl;

    //editorState->selectedNode->SetPosition( position );
    //editorState->selectedNode->SetRotation( rotation );
}
