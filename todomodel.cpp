#include "todomodel.h"
#include "todolist.h"

// Конструктор модели задач
TodoModel::TodoModel(QObject *parent)
    : QAbstractListModel(parent), mList(nullptr) // Инициализация родительского класса и указателя на список задач
{
}

// Возвращает количество строк в модели
int TodoModel::rowCount(const QModelIndex &parent) const
{
    // Если индекс родителя не валиден или список задач не установлен, возвращаем 0
    if (parent.isValid() || !mList)
        return 0;

    // Возвращаем количество задач в списке
    return mList->get_tasks().size();
}

// Возвращает данные для указанного индекса и роли
QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    // Проверяем, валиден ли индекс и установлен ли список задач
    if (!index.isValid() || !mList)
        return QVariant(); // Возвращаем пустой QVariant, если индекс не валиден

    // Получаем задачу по индексу
    const Task task = mList->get_tasks().at(index.row());

    // Возвращаем данные в зависимости от роли
    switch (role)
    {
    case DoneRole:
        return QVariant(task.done); // Возвращаем статус выполнения задачи
    case DescriptionRole:
        return QVariant(task.description); // Возвращаем описание задачи
    case HeadingRole:
        return QVariant(task.heading); // Возвращаем заголовок задачи
    case DateRole:
        return QVariant(task.date); // Возвращаем дату задачи
    case IdRole:
        return QVariant(task.id); // Возвращаем идентификатор задачи
    }

    return QVariant(); // Возвращаем пустой QVariant, если роль не распознана
}

// Устанавливает данные для указанного индекса и роли
bool TodoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Проверяем, установлен ли список задач
    if (!mList)
        return false; // Возвращаем false, если список не установлен

    // Получаем задачу по индексу
    Task task = mList->get_tasks().at(index.row());

    // Обновляем данные задачи в зависимости от роли
    switch (role)
    {
    case DoneRole:
        task.done = value.toBool(); // Устанавливаем статус выполнения
        break;
    case HeadingRole:
        task.heading = value.toString(); // Устанавливаем заголовок задачи
        break;
    case DescriptionRole:
        task.description = value.toString(); // Устанавливаем описание задачи
        break;
    case DateRole:
        task.date = value.toString(); // Устанавливаем дату задачи
        break;
    case IdRole:
        task.id = value.toUInt(); // Устанавливаем идентификатор задачи
        break;
    }

    // Если задача успешно обновлена в списке, генерируем сигнал о изменении данных
    if (mList->set_task_at(index.row(), task))
    {
        emit dataChanged(index, index, QVector<int>() << role); // Генерируем сигнал о изменении данных
        return true;                                            // Возвращаем true, если обновление прошло успешно
    }
    return false; // Возвращаем false, если обновление не удалось
}

// Возвращает флаги для элемента по индексу
Qt::ItemFlags TodoModel::flags(const QModelIndex &index) const
{
    // Если индекс не валиден, возвращаем флаги отсутствия элементов
    if (!index.isValid())
        return Qt::NoItemFlags;

    // Возвращаем флаг редактируемости
    return Qt::ItemIsEditable;
}

// Возвращает имена ролей для модели
QHash<int, QByteArray> TodoModel::roleNames() const
{
    QHash<int, QByteArray> names;           // Создаем хэш для имен ролей
    names[DoneRole] = "done";               // Роль для статуса выполнения
    names[DescriptionRole] = "description"; // Роль для описания задачи
    names[HeadingRole] = "heading";         // Роль для заголовка задачи
    names[DateRole] = "date";               // Роль для даты задачи
    names[IdRole] = "id";                   // Роль для идентификатора задачи

    return names; // Возвращаем хэш с именами ролей
}

// Возвращает указатель на список задач
TodoList *TodoModel::list() const
{
    return mList; // Возвращаем текущий список задач
}

// Устанавливает список задач для модели
void TodoModel::setList(TodoList *list)
{
    beginResetModel(); // Начинаем сброс модели

    // Отключаем предыдущие соединения, если список уже установлен
    if (mList)
        mList->disconnect(this);

    mList = list; // Устанавливаем новый список задач

    // Если новый список задач установлен, подключаем сигналы
    if (mList)
    {
        // Подключаем сигнал перед добавлением задачи
        connect(mList, &TodoList::pre_task_appended, this, [=]()
                {
                    const int index = mList->get_tasks().size();  // Получаем индекс новой задачи
                    beginInsertRows(QModelIndex(), index, index); // Начинаем вставку строки
                });

        // Подключаем сигнал после добавления задачи
        connect(mList, &TodoList::post_task_appended, this, [=]()
                { endInsertRows(); }); // Завершаем вставку строки

        // Подключаем сигналы для сброса модели
        connect(mList, &TodoList::pre_task_reset, this, [=]()
                { beginResetModel(); }); // Начинаем сброс модели
        connect(mList, &TodoList::post_task_reset, this, [=]()
                { endResetModel(); }); // Завершаем сброс модели

        // Подключаем сигнал перед удалением задачи
        connect(mList, &TodoList::pre_task_removed, this, [=](int index)
                { beginRemoveRows(QModelIndex(), index, index); }); // Начинаем удаление строки

        // Подключаем сигнал после удаления задачи
        connect(mList, &TodoList::post_task_removed, this, [=]()
                { endRemoveRows(); }); // Завершаем удаление строки
    }

    endResetModel(); // Завершаем сброс модели
}
