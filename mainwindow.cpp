#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define LIST_NAME "lr2_list.txt"
#define INITIAL_USES 5
int save_num = 0;
bool is_premium = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Инициализируем UI
    ui->setupUi(this);
    //проверяем доступ к приложению
    check_trial();
    this->setWindowTitle(is_premium ? "TIMP LR2 Mega Ultra Premium+ Edition" : "TIMP LR2");
    //в качестве стандартного пути для файла выставляем папку с .exe приложения
    ui->directory_textbox->setText(QCoreApplication::applicationDirPath());

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_directory_button_clicked()
{
    //Смена директории при нажатии на кнопку
    QString new_dir_name = QFileDialog::getExistingDirectory(this, "Выберите директорию", ui->directory_textbox->text(), QFileDialog::ShowDirsOnly);
    if(!new_dir_name.isEmpty()){
        ui->directory_textbox->setText(new_dir_name);
    }
}


void MainWindow::on_name_button_clicked()
{
    //Проверяем введенное имя
    if(ui->name_textbox->text().isEmpty()){
        QMessageBox::critical(this, "Ошибка", "Вы не указали Ф.И.О для сохранения");
        return;
    }
    QFile file(ui->directory_textbox->text()+"/"+LIST_NAME);
    //проверяем существование файла
    if(file.exists()){
        //Если он есть - пытаемся считать
        if(!file.open(QIODevice::ReadWrite)){
            QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл.");
            QCoreApplication::quit();
             return;
        }
        //Открываем поток для считывания
        QTextStream in(&file);
        while(!in.atEnd()){
            //считываем построчно
            QString line = in.readLine();
            if(line == ui->name_textbox->text()){
                QMessageBox::information(this, "Внимание", "В списке уже содержится такое Ф.И.О.");
                QCoreApplication::quit();
                return;
            }
        }
        //Если в файле нет такой фамилии - записываем, и помечаем запуск как успешный
        in << ui->name_textbox->text() << Qt::endl;
        this->successful = true;
    } else {
        //если файла нет - создаём
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::critical(this, "Ошибка", "Не удалось создать файл.");
            QCoreApplication::quit();
            return;
        }
        //Записываем имя в файл
        QTextStream out(&file);
        out << ui->name_textbox->text() << Qt::endl;
        file.close();
        this->successful = true;
    }

    QCoreApplication::quit();
}

/////////////////////////////////////////////////////////
bool check_Key(QString key){
    //Ключ должен быть длиной в 8 цифр, и третья и последняя цифра должны быть одинаковыми
    if(key.length()!= 8 || key[2]!=key[7]) return false;
    int sum = 0;
    //Проверяем, что все символы - цифры
    for(int i = 0; i < 8; i++){
        if(!key[i].isDigit()) return false;
        else if(i < 3) continue;
        //считаем сумму всех цифр с 4 по 10ю
        sum+=key[i].digitValue();
    }
    //если сумма числа, полученного из первых трёх цифр, и суммы цифер с 4 по 8, кратно 47 - ключ верный
    int first_three_digits = key.left(3).toInt();
    //qDebug() << first_three_digits << " " << sum;
    if((sum+first_three_digits)%47 == 0) return true;
    else return false;
}

void MainWindow::check_trial(){
    QSettings settings("thevals", "TIMP LR2");
    //В Windows это сохраняет в реестре HKEY_CURRENT_USER\Software\thevals\TIMP LR2
    //проверяем указан ли ключ в реестре
    if(settings.contains("Key")){
        if(check_Key(QString::number(settings.value("Key").toInt()))){
            //если ключ верен - включаем премиум версию
            is_premium = true;
        } else {
            QMessageBox::critical(this, "Ошибка", "Сохранённый ключ невалиден. Возможно, вы пытались его изменить сторонними способами. Программа будет удалена");
            start_uninstalling();
        }
    } else if(settings.contains("Config")){
        //парсим значение Config
            int num = settings.value("Config").toInt();
        //чтобы значения количества запусков не было так легко понять и изменить в реестре - они немного спрятаны через математику
            //получаются эти значения следующим образом: ((x+100)*4-3)*337. из этого можем получить несколько ограничений и проверок
            if((num < 0) || (num%337 != 0) || (num/337%2 == 0) || (((num/337)+3)/4 <= 100)){
            //если в них какая то ошибка - заставляем ввести ключ или удалить приложение
                    QMessageBox msg_box;
                    //Создаем окно с сообщением и задаём ему параметры
                    msg_box.setText("Некорректные значения данных об использовании. Походу, вы пытались взломать меня, чтобы пользоваться пробной версией дальше :) К сожалению, это так не работает. Желаете ввести ключ?");
                    msg_box.setWindowTitle("Ошибка");
                    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msg_box.setDefaultButton(QMessageBox::Yes);
                    int ret = msg_box.exec();
                    if(ret == QMessageBox::Yes){
                //  если пользователь согласился ввести пароль - открываем окно для ввода
                        QString key_text = QInputDialog::getText(this, "Ввод","Введите ключ", QLineEdit::Normal);
                        if(check_Key(key_text)){
                            is_premium = true;
                            settings.setValue("Key", key_text.toInt());
                            //если пароль подошел - записываем его в реестр и выдаем премиум статус этому запуску. иначе - удаляем программу.
                        } else {
                            QMessageBox::critical(this, "Неудача", "Ввёденный ключ невалиден. Программа будет удалена.");
                            start_uninstalling();
                        }

                    } else start_uninstalling();
            } else {
                //если запись о количестве запусков корректная - проверяем, не превышает ли их количество заданное
                num=(((num/337)+3)/4)-100;
                //qDebug() << num;
                if(num >= INITIAL_USES){
                        //если превышает -предлагаем ввести ключ или удалить программу
                    QMessageBox msg_box;
                    msg_box.setText("Вы превысили количество бесплатных использований приложения. Вы можете купить ключ у разработчика (telegram: @thevals), и нажать 'Да', чтобы ввести его. В противном случае, программа запустит процесс самоудаления.");
                    msg_box.setWindowTitle("Внимание");
                    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msg_box.setDefaultButton(QMessageBox::Yes);
                    int ret = msg_box.exec();
                    //аналогично, создаем информационное окно и задаём ему параметры
                    if(ret == QMessageBox::Yes){
                            //если согласились ввести ключ - создаем диалоговое окно и проверяем введённый ключ
                        QString key_text = QInputDialog::getText(this, "Ввод","Введите ключ", QLineEdit::Normal);
                        if(check_Key(key_text)){
                            is_premium = true;
                            settings.setValue("Key", key_text.toInt());
                        } else {
                            QMessageBox::critical(this, "Неудача", "Ввёденный ключ невалиден. Программа будет удалена.");
                            start_uninstalling();
                        }

                    } else start_uninstalling();
                } else {
                    ui->trial_text_label->setText("Осталось бесплатных использований: " + QString::number(INITIAL_USES - num));
                }
                save_num = num+1;
            }
    } else {
            save_num=1;
    }
}

void MainWindow::start_uninstalling(){
    //создаём новый процесс - деинстяллятор с аргументами silent удаления
    QString program = QCoreApplication::applicationDirPath()+"/maintenancetool.exe";

    if(QFile::exists(program)) {
        QStringList arguments;
        arguments << "purge" << "--confirm-command";
        QProcess::startDetached(program, arguments);
    } else {
        qDebug() << "uninstaller not found or failed to launch";
    }
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    //перехватываем закрытие приложения, чтобы записывать новые значения в реестр в случае полностью успешного выполнения
    if (this->successful)
    {
        //Засчитываем использования только в случае успеха
        QMessageBox::information(this, "Успех", is_premium? "Спасибо за использование премиум версии программы" : ("Осталось использований пробной версии: " + QString::number(save_num)));
        if(!is_premium){
        QSettings settings("thevals", "TIMP LR2");
        int num = ((((save_num)+100)*4-3)*337);
        settings.setValue("Config", num);
        }
    }
    event->accept();
}
