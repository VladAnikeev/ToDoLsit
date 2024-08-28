#include "todolist.h"

TodoList::TodoList(QObject *parent) : QObject(parent) {}

QVector<Task> TodoList::get_tasks() const
{
    return tasks;
}

// изменение по индексу
bool TodoList::set_task_at(unsigned int index, const Task &change_task)
{
    // проверка на изменения
    if (index < 0 || index >= tasks.size() || tasks[index] == change_task)
        return false;

    // изменение
    tasks[index] = change_task;

    // не забываем обновить всю задачу, если она находятся в резервной копии
    auto it = std::find_if(reserve_tasks.begin(), reserve_tasks.end(),
                           [&change_task](const Task &task)
                           {
                               return task.id == change_task.id;
                           });
    if (it != reserve_tasks.end())
        *it = change_task;

    return true;
}

// Добавить пустую задачу
void TodoList::append_task()
{
    Task task(++last_id);
    append_task(task);
}

// Добавить задачу
void TodoList::append_task(Task &task)
{
    emit pre_task_appended();
    tasks.append(task);
    emit post_task_appended();

    // добавить в резерв, если он используется
    if (!reserve_tasks.isEmpty())
        reserve_tasks.append(task);

    last_id = task.id;
}

// Удалить задачу по индексу
void TodoList::remove(unsigned int index)
{
    emit pre_task_removed(index);
    tasks.removeAt(index);
    emit post_task_removed();
}

// удаление выполненных
void TodoList::remove_completed_tasks()
{
    for (int i = 0; i < tasks.size();)
    {
        if (tasks[i].done)
        {
            // удаляем из резерва
            for (int j = 0; j < reserve_tasks.size(); j++)
            {
                if (reserve_tasks[j].id == tasks[i].id)
                {
                    reserve_tasks.removeAt(j);
                    break;
                }
            }
            // удаляем из отображаемо вектора
            remove(i);
        }
        else
        {
            i++;
        }
    }
}

void TodoList::save()
{
    // сформируем массив из объектов
    QJsonArray tasks_array;

    for (auto &task : tasks)
    {
        QJsonObject obj_task;
        obj_task.insert("id", QJsonValue::fromVariant(task.id));
        obj_task.insert("done", QJsonValue::fromVariant(task.done));
        obj_task.insert("heading", QJsonValue::fromVariant(task.heading));
        obj_task.insert("description", QJsonValue::fromVariant(task.description));
        obj_task.insert("date", QJsonValue::fromVariant(task.date));

        tasks_array.append(obj_task);
    }

    // создаем основной JSON объект и добавляем массив задач
    QJsonObject main_json;
    main_json.insert("tasks", tasks_array); // Добавляем массив задач в основной объект

    // преобразуем QJsonObject в QJsonDocument
    QJsonDocument document(main_json);
    // сериализуем в JSON строку с отступами
    QString json_string = document.toJson(QJsonDocument::Indented);

    // создаем файл и записываем в него JSON
    QFile file(FILE_SAVE);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << json_string;
        file.close();
        qDebug() << "Файл успешно записан: " << FILE_SAVE;
    }
    else
    {
        qDebug() << "Не удалось открыть файл для записи";
    }
}

void TodoList::load()
{
    QFile file(FILE_SAVE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл для чтения:" << file.errorString();
        return;
    }

    QByteArray file_data = file.readAll(); // читаем все данные из файла
    file.close();                          // закрываем файл

    QJsonDocument document = QJsonDocument::fromJson(file_data); // преобразуем данные в QJsonDocument
    if (document.isNull())
    {
        qDebug() << "Ошибка при парсинге JSON";
        return;
    }

    if (!document.isObject())
    {
        qDebug() << "JSON не является объектом";
        return;
    }

    QJsonObject main_json = document.object();
    QJsonArray tasks_array = main_json.value("tasks").toArray(); // получаем массив задач

    // если есть задачи в файле
    if (!tasks_array.isEmpty())
    {
        // добавляем задачи
        for (const QJsonValue &value : tasks_array)
        {
            if (value.isObject())
            {
                QJsonObject obj_task = value.toObject();
                Task newTask;
                newTask.id = obj_task.value("id").toVariant().toInt();
                newTask.done = obj_task.value("done").toVariant().toBool();
                newTask.heading = obj_task.value("heading").toVariant().toString();
                newTask.description = obj_task.value("description").toVariant().toString();
                newTask.date = obj_task.value("date").toVariant().toString();

                append_task(newTask);
            }
        }

        last_id = tasks.last().id;
    }
}

void TodoList::reset()
{

    // проверяем, есть ли элементы в резерве
    if (reserve_tasks.isEmpty())
        return;

    pre_task_reset();

    // очищаем отображаемый список
    tasks.clear();

    post_task_reset();
    for (auto &task : reserve_tasks)
    {
        emit pre_task_appended();

        tasks.append(task);

        emit post_task_appended();
    }
    // добавляем элемент в список
    // очищаем резерв
    reserve_tasks.clear();
}

void TodoList::find(
    const bool &done,
    const QString &heading,
    const QString &description,
    const QString &from_date_string,
    const QString &before_date_string)
{
    // используем резерв
    if (reserve_tasks.isEmpty())
    {
        reserve_tasks = tasks;
    }

    QDate from_date = QDate::fromString(from_date_string, FORMAT_DATE);
    QDate before_date = QDate::fromString(before_date_string, FORMAT_DATE);

    // если пуст шаблон - не искать
    if (!done &&
        heading.isEmpty() &&
        description.isEmpty() &&
        from_date.isValid() &&
        before_date.isValid())
    {
        return;
    }
    // удаляем из отображаемого списка
    for (int i = 0; i < tasks.size();)
    {
        QDate task_date = QDate::fromString(tasks.at(i).date, FORMAT_DATE);

        if (tasks.at(i).done != done ||
            tasks.at(i).heading.indexOf(heading) == -1 ||
            tasks.at(i).description.indexOf(description) == -1 ||
            // проверка даты
            (task_date.isValid() &&
             (from_date.isValid() && from_date > task_date ||
              before_date.isValid() && before_date < task_date)))
        {
            remove(i);
        }
        else
        {
            ++i;
        }
    }
}
