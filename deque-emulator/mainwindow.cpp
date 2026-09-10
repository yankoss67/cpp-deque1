#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algo.h"

#include <deque>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <algorithm>
#include <random>
#include <string>

static std::deque<std::string> tea {
    "Чай Лунцзин",
    "Эрл Грей",
    "Сенча",
    "Пуэр",
    "Дарджилинг",
    "Ассам",
    "Матча",
    "Ганпаудер",
    "Оолонг",
    "Лапсанг Сушонг"
};

static std::deque<std::string> cakes {
    "Красный бархат",
    "Наполеон",
    "Медовик",
    "Тирамису",
    "Прага",
    "Чизкейк",
    "Захер",
    "Эстерхази",
    "Морковный торт",
    "Чёрный лес",
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    std::random_device rd;
    random_gen_ = std::mt19937(rd());

    ApplyModel();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::SetRandomGen(const std::mt19937& random_gen) {
    random_gen_ = random_gen;
}

void MainWindow::ApplyModel() {
    // Сохраняем ПОЗИЦИЮ (индекс)
    int index_tmp = -1;
    if (deque_model_.iterator != deque_model_.items.end()) {
        index_tmp = std::distance(deque_model_.items.begin(), deque_model_.iterator);
    }
    
    ui->list_widget->clear();
    for (size_t i = 0; i < deque_model_.items.size(); ++i) {
        ui->list_widget->addItem(QString::number(i) + ": " +
                                 QString::fromStdString(deque_model_.items[i]));
    }
    ui->list_widget->addItem("end");
    ui->txt_size->setText(QString::number(deque_model_.items.size()));
    
    // Восстанавливаем ПОЗИЦИЮ (индекс)
    if (index_tmp >= 0 && index_tmp < static_cast<int>(deque_model_.items.size())) {
        deque_model_.iterator = deque_model_.items.begin() + index_tmp;
    } else if (!deque_model_.items.empty()) {
        deque_model_.iterator = deque_model_.items.begin();
    } else {
        deque_model_.iterator = deque_model_.items.end();
    }
    
    bool is_empty = deque_model_.items.empty();
    ui->btn_pop_back->setDisabled(is_empty);
    ui->btn_pop_front->setDisabled(is_empty);
    ui->btn_erase->setDisabled(is_empty);
    ui->btn_edit->setDisabled(is_empty);
    
    ApplyIterator();
}

void MainWindow::ApplyIterator() {
    int index = std::distance(deque_model_.items.begin(), deque_model_.iterator);
    ui->list_widget->setCurrentRow(index);

    bool disabled = (deque_model_.iterator == deque_model_.items.end());

    if (!disabled) {
        ui->txt_elem_content->setText(QString::fromStdString(*deque_model_.iterator));
        ui->txt_elem_content->setEnabled(true);
    } else {
        ui->txt_elem_content->clear();
        ui->txt_elem_content->setEnabled(false);
    }

    ui->btn_dec_iterator->setDisabled(deque_model_.iterator == deque_model_.items.begin());
    ui->btn_inc_iterator->setDisabled(disabled);
    ui->btn_erase->setDisabled(disabled);
    ui->btn_edit->setDisabled(disabled);
}
// ==================== МЕТОДЫ ДЕКА ====================

void MainWindow::on_btn_push_back_clicked() {
    QString text = ui->txt_elem_content->text();
    if (!text.isEmpty()) {
        deque_model_.items.push_back(text.toStdString());
        deque_model_.iterator = deque_model_.items.begin();
        ApplyModel();
    }
}

void MainWindow::on_btn_push_front_clicked() {
    QString text = ui->txt_elem_content->text();
    if (!text.isEmpty()) {
        deque_model_.items.push_front(text.toStdString());
        deque_model_.iterator = deque_model_.items.begin();
        ApplyModel();
        ApplyIterator();
    }
}

void MainWindow::on_btn_pop_back_clicked() {
    if (!deque_model_.items.empty()) {
        deque_model_.items.pop_back();
        if (!deque_model_.items.empty()) {
            deque_model_.iterator = deque_model_.items.begin();
        } else {
            deque_model_.iterator = deque_model_.items.end();
        }
        ApplyModel();
        ApplyIterator();
    }
}
void MainWindow::on_btn_pop_front_clicked() {
    if (!deque_model_.items.empty()) {
        deque_model_.items.pop_front();
        if (!deque_model_.items.empty()) {
            deque_model_.iterator = deque_model_.items.begin();
        } else {
            deque_model_.iterator = deque_model_.items.end();
        }
        ApplyModel();
        ApplyIterator();
    }
}

void MainWindow::on_btn_clear_clicked() {
    deque_model_.items.clear();
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
    ApplyIterator();
}

void MainWindow::on_btn_insert_clicked() {
    QString text = ui->txt_elem_content->text();
    if (!text.isEmpty()) {
        deque_model_.items.insert(deque_model_.iterator, text.toStdString());
        if (!deque_model_.items.empty()) {
            deque_model_.iterator = deque_model_.items.begin();
        } else {
            deque_model_.iterator = deque_model_.items.end();
        }
        ApplyModel();
        ApplyIterator();
    }
}

void MainWindow::on_btn_erase_clicked() {
    if (deque_model_.iterator != deque_model_.items.end()) {
        deque_model_.items.erase(deque_model_.iterator);
        if (!deque_model_.items.empty()) {
            deque_model_.iterator = deque_model_.items.begin();
        } else {
            deque_model_.iterator = deque_model_.items.end();
        }
        ApplyModel();
        ApplyIterator();
    }
}

void MainWindow::on_btn_edit_clicked() {
    if (deque_model_.iterator != deque_model_.items.end()) {
        QString text = ui->txt_elem_content->text();
        if (!text.isEmpty()) {
            *deque_model_.iterator = text.toStdString();
            ApplyModel();
            ApplyIterator();
        }
    }
}

void MainWindow::on_btn_resize_clicked() {
    QString size_text = ui->txt_size->text();
    if (size_text.isEmpty()) return;

    bool ok;
    size_t new_size = size_text.toUInt(&ok);
    if (!ok) return;

    if (new_size > deque_model_.items.size()) {
        while (deque_model_.items.size() < new_size) {
            deque_model_.items.push_back("");
        }
    } else if (new_size < deque_model_.items.size()) {
        while (deque_model_.items.size() > new_size) {
            deque_model_.items.pop_back();
        }
    }

    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

// ==================== ИТЕРАТОР ====================

void MainWindow::on_btn_begin_clicked() {
    if (!deque_model_.items.empty()) {
        deque_model_.iterator = deque_model_.items.begin();
        ApplyIterator();
    }
}

void MainWindow::on_btn_end_clicked() {
    deque_model_.iterator = deque_model_.items.end();
    ApplyIterator();
}

void MainWindow::on_btn_dec_iterator_clicked() {
    if (deque_model_.iterator != deque_model_.items.begin()) {
        --deque_model_.iterator;
        ApplyIterator();
    }
}

void MainWindow::on_btn_inc_iterator_clicked() {
    if (deque_model_.iterator != deque_model_.items.end()) {
        ++deque_model_.iterator;
        ApplyIterator();
    }
}

// ==================== ЗАГОТОВКИ ====================

void MainWindow::on_btn_tea_clicked() {
    deque_model_.items = tea;
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_cakes_clicked() {
    deque_model_.items = cakes;
    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

// ==================== АЛГОРИТМЫ ====================

void MainWindow::on_btn_find_clicked() {
    QString search_text = ui->txt_elem_content->text();
    if (search_text.isEmpty() || deque_model_.items.empty()) {
        return;
    }

    std::string search_str = search_text.toStdString();
    auto it = std::find(deque_model_.items.begin(), deque_model_.items.end(), search_str);

    deque_model_.iterator = it;
    ApplyIterator();
}

void MainWindow::on_btn_count_clicked() {
    QString search_text = ui->le_count->text();
    if (search_text.isEmpty() || deque_model_.items.empty()) {
        ui->lbl_count->setText("0");
        return;
    }

    std::string search_str = search_text.toStdString();
    size_t count = std::count(deque_model_.items.begin(), deque_model_.items.end(), search_str);
    ui->lbl_count->setText(QString::number(count));
}

void MainWindow::on_btn_min_element_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }
    auto it = std::min_element(deque_model_.items.begin(), deque_model_.items.end());
    deque_model_.iterator = it;
    ApplyIterator();
}

void MainWindow::on_btn_max_element_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }
    auto it = std::max_element(deque_model_.items.begin(), deque_model_.items.end());
    deque_model_.iterator = it;
    ApplyIterator();
}

void MainWindow::on_btn_merge_sort_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    deque_model_.items = MergeSort(deque_model_.items,
                                   [](const std::string& a, const std::string& b) {
                                       return a < b;
                                   });

    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_merge_sort_case_insensitive_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    deque_model_.items = MergeSort(deque_model_.items,
                                   [](const std::string& a, const std::string& b) {
                                       QString s1 = QString::fromStdString(a);
                                       QString s2 = QString::fromStdString(b);
                                       return QString::compare(s1, s2, Qt::CaseInsensitive) < 0;
                                   });

    deque_model_.iterator = deque_model_.items.begin();
    ApplyModel();
}

void MainWindow::on_btn_shuffle_clicked() {
    if (deque_model_.items.empty()) return;
    
    std::vector<std::string> temp(deque_model_.items.begin(), deque_model_.items.end());
    std::shuffle(temp.begin(), temp.end(), random_gen_);
    deque_model_.items.assign(temp.begin(), temp.end());
    
    ApplyModel();
}

void MainWindow::on_btn_unique_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    if (!std::is_sorted(deque_model_.items.begin(), deque_model_.items.end())) {
        return;
    }

    auto new_end = std::unique(deque_model_.items.begin(), deque_model_.items.end());
    deque_model_.items.erase(new_end, deque_model_.items.end());

    deque_model_.iterator = deque_model_.items.begin();
    
    ApplyModel();
}

void MainWindow::on_btn_reverse_clicked() {
    if (deque_model_.items.empty()) return;
    
    std::reverse(deque_model_.items.begin(), deque_model_.items.end());
    
    ApplyModel();
}

// ==================== ALGORITHMS ====================

void MainWindow::on_btn_lower_bound_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    if (!std::is_sorted(deque_model_.items.begin(), deque_model_.items.end())) {
        return;
    }

    QString search_text = ui->txt_elem_content->text();
    if (search_text.isEmpty()) {
        return;
    }

    std::string search_str = search_text.toStdString();
    auto it = std::lower_bound(deque_model_.items.begin(), deque_model_.items.end(), search_str);

    deque_model_.iterator = it;
    ApplyIterator();
}

void MainWindow::on_btn_upper_bound_clicked() {
    if (deque_model_.items.empty()) {
        return;
    }

    if (!std::is_sorted(deque_model_.items.begin(), deque_model_.items.end())) {
        return;
    }

    QString search_text = ui->txt_elem_content->text();
    if (search_text.isEmpty()) {
        return;
    }

    std::string search_str = search_text.toStdString();
    auto it = std::upper_bound(deque_model_.items.begin(), deque_model_.items.end(), search_str);

    deque_model_.iterator = it;
    ApplyIterator();
}

// ==================== ВЫДЕЛЕНИЕ В СПИСКЕ ====================

void MainWindow::on_list_widget_currentRowChanged(int currentRow) {
    size_t size = deque_model_.items.size();

    if (currentRow >= 0 && static_cast<size_t>(currentRow) < size) {
        deque_model_.iterator = deque_model_.items.begin() + currentRow;
        ApplyIterator();
    } else if (currentRow == static_cast<int>(size)) {
        deque_model_.iterator = deque_model_.items.end();
        ApplyIterator();
    }
}
