#ifndef TODOLIST_H
#define TODOLIST_H

#include <QDateTime>
#include <QObject>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

#define FILE_SAVE "./data_base.json"
#define FORMAT_DATE "dd/MM/yyyy"

class Task
{
public:
    unsigned int id; // не лучшая реализация для id, лучше использовать уникальные ключи ключи
    bool done;
    QString heading;
    QString description;
    QString date;

    Task() {};

    Task(unsigned int _id,
         bool _done = false,
         QString _heading = "",
         QString _description = "",
         QString _date = "") : id(_id), done(_done), heading(_heading), description(_description)
    {
    }

    bool operator==(Task &right_task)
    {
        return done == right_task.done &&
               description == right_task.description &&
               heading == right_task.heading &&
               date == right_task.date;
    }
    bool operator==(const Task &right_task)
    {
        return done == right_task.done &&
               description == right_task.description &&
               heading == right_task.heading &&
               date == right_task.date;
    }
};

class TodoList : public QObject
{
    Q_OBJECT
public:
    explicit TodoList(QObject *parent = nullptr);

    QVector<Task> get_tasks() const;
    bool set_task_at(unsigned int index, const Task &change_task);

    unsigned int last_id = 0;
public slots:

    void append_task();
    void append_task(Task &task);
    void remove_completed_tasks();
    void remove(unsigned int index);

    void reset();
    void find(const bool &done,
              const QString &heading,
              const QString &description,
              const QString &from_date_string,
              const QString &before_date_string);

    void save();
    void load();

signals:
    void pre_task_appended();
    void post_task_appended();

    void pre_task_reset();
    void post_task_reset();

    void pre_task_removed(unsigned int index);
    void post_task_removed();

private:
    QVector<Task> tasks;
    QVector<Task> reserve_tasks; // копия, используется для поиска
};

#endif // TODOLIST_H
