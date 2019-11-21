#ifndef HYODURGAMODULEFORM_H
#define HYODURGAMODULEFORM_H

#include <QWidget>

namespace Ui {
class HyoDurgaModuleForm;
}

class QComboBox;
class HyoDurgaModuleForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit HyoDurgaModuleForm(QWidget *parent = 0);
    ~HyoDurgaModuleForm();

    QString model() const;

    QVariant getValue() const;
    void setValue(const QVariant &value);
    void setValue(int idx, bool value);

    bool eventFilter(QObject *object, QEvent *event);
    
private:
    Ui::HyoDurgaModuleForm *ui;
    QComboBox* m_comboBox;
    QWidget* m_valueWidget;

    void init();

private slots:
    void onModelChanged(const QString &model);

    void onSpinBoxClicked();
    void onNumberPadEntered(const QVariant &val);
    void onNumberPadCanceled();
};

#endif // HYODURGAMODULEFORM_H
