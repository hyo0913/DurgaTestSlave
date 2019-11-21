#include "HyoDurgaModuleForm.h"
#include "ui_HyoDurgaModuleForm.h"

#include <QComboBox>
#include <QSpinBox>

#include "DurgaTestCommon.h"
#include "HyoDioForm.h"
#include "HyoNumberPad.h"

HyoDurgaModuleForm::HyoDurgaModuleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HyoDurgaModuleForm)
{
    ui->setupUi(this);

    init();

    this->adjustSize();
}

HyoDurgaModuleForm::~HyoDurgaModuleForm()
{
    delete ui;
}

QString HyoDurgaModuleForm::model() const
{
    return m_comboBox->currentText();
}

void HyoDurgaModuleForm::init()
{
    m_comboBox = new QComboBox();
    ui->horizontalLayout->addWidget(m_comboBox);

    m_comboBox->addItem(ModelName_DI);
    m_comboBox->addItem(ModelName_DO);
    m_comboBox->addItem(ModelName_AI);

    connect(m_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onModelChanged(QString)));

    HyoDioForm* dio = new HyoDioForm(8);
    ui->horizontalLayout->addWidget(dio);
    m_valueWidget = dio;

    QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->horizontalLayout->addSpacerItem(spacer);
}

QVariant HyoDurgaModuleForm::getValue() const
{
    QString model = m_comboBox->currentText();
    QVariant value;

    if( model == ModelName_DI || model == ModelName_DO ) {
        value.setValue(static_cast<HyoDioForm*>(m_valueWidget)->getValue());
    } else if( model == ModelName_AI || model == ModelName_AO ) {
        value.setValue(static_cast<QSpinBox*>(m_valueWidget)->value());
    }

    return value;
}

void HyoDurgaModuleForm::setValue(const QVariant &value)
{
    QString model = m_comboBox->currentText();

    if( model == ModelName_DI || model == ModelName_DO ) {
        static_cast<HyoDioForm*>(m_valueWidget)->setValue(value.toULongLong());
    } else if( model == ModelName_AI || model == ModelName_AO ) {
        static_cast<QSpinBox*>(m_valueWidget)->setValue(value.toFloat());
    }
}

void HyoDurgaModuleForm::setValue(int idx, bool value)
{
    QString model = m_comboBox->currentText();

    if( model == ModelName_DI || model == ModelName_DO ) {
        static_cast<HyoDioForm*>(m_valueWidget)->setValue(idx, value);
    } else if( model == ModelName_AI || model == ModelName_AO ) {
    }
}

bool HyoDurgaModuleForm::eventFilter(QObject *object, QEvent *event)
{
    if( event->type() == QEvent::HoverEnter )
    {
        onSpinBoxClicked();
    }

    return QWidget::eventFilter(object, event);
}

void HyoDurgaModuleForm::onModelChanged(const QString &model)
{
    if( m_valueWidget != NULL )
    {
        ui->horizontalLayout->removeWidget(m_valueWidget);
        delete m_valueWidget;
        m_valueWidget = NULL;
    }

    if( model == ModelName_DI || model == ModelName_DO ) {
        HyoDioForm* dio = new HyoDioForm(8);
        ui->horizontalLayout->insertWidget(1, dio);
        m_valueWidget = dio;
    } else if( model == ModelName_AI || model == ModelName_AO ) {
        QSpinBox* spinBox = new QSpinBox();
        spinBox->setObjectName("AnalogValue");
        spinBox->setMinimum(0);
        spinBox->setMaximum(65535);
        ui->horizontalLayout->insertWidget(1, spinBox);
        m_valueWidget = spinBox;

        spinBox->installEventFilter(this);
    }
}

void HyoDurgaModuleForm::onSpinBoxClicked()
{
    HyoNumberPad* numberPad = new HyoNumberPad();

    connect(numberPad, SIGNAL(entered(QVariant)), this, SLOT(onNumberPadEntered(QVariant)));
    connect(numberPad, SIGNAL(canceled()), this, SLOT(onNumberPadCanceled()));

    numberPad->setAttribute(Qt::WA_DeleteOnClose);
    numberPad->setModal(true);
    numberPad->show(static_cast<QSpinBox*>(m_valueWidget)->value());

    QPoint pos = this->cursor().pos();
    numberPad->move(pos);
}

void HyoDurgaModuleForm::onNumberPadEntered(const QVariant &val)
{
    static_cast<QSpinBox*>(m_valueWidget)->setValue(val.toInt());
    m_valueWidget->clearFocus();
}

void HyoDurgaModuleForm::onNumberPadCanceled()
{
    m_valueWidget->clearFocus();
}
