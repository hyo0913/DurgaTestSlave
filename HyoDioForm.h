#ifndef HYODIOFORM_H
#define HYODIOFORM_H

#include <QWidget>

namespace Ui {
class HyoDioForm;
}

class QPushButton;
class HyoDioForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit HyoDioForm(int points, QWidget *parent = 0);
    ~HyoDioForm();

    quint64 getValue() const;
    bool getValue(int idx) const;
    void setValue(int idx, bool val);
    void setValue(quint64 val);
    
private:
    Ui::HyoDioForm *ui;
    QList<QPushButton*> m_points;

    void makePoints(int points);
};

#endif // HYODIOFORM_H
