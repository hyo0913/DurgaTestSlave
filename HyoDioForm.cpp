#include "HyoDioForm.h"
#include "ui_HyoDioForm.h"

#include <QHBoxLayout>
#include <QPushButton>

HyoDioForm::HyoDioForm(int points, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HyoDioForm)
{
    ui->setupUi(this);

    makePoints(points);

    this->adjustSize();
}

HyoDioForm::~HyoDioForm()
{
    delete ui;
}

void HyoDioForm::makePoints(int points)
{
    QHBoxLayout* layout = NULL;

    for( int i = 0; i < points; i++ )
    {
        if( i% 8 == 0 ) {
            layout = new QHBoxLayout();
            ui->verticalLayout->addLayout(layout);
        }

        QPushButton* pushButton = new QPushButton();
        layout->addWidget(pushButton);
        m_points.append(pushButton);

        pushButton->setText(QString(""));
        QSize size(21, 21);
        pushButton->setMaximumSize(size);
        pushButton->setMinimumSize(size);

        pushButton->setCheckable(true);

        QIcon icon;
        icon.addFile(":/Icon/IconLampOff", size, QIcon::Normal, QIcon::Off);
        icon.addFile(":/Icon/IconLampOn", size, QIcon::Normal, QIcon::On);
        pushButton->setIcon(icon);

        if( i > 0 && i % 8 == 0 ) {
            QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
            layout->addSpacerItem(spacer);
        }
    }
}

quint64 HyoDioForm::getValue() const
{
    quint64 val = 0;

    for( int i = m_points.count()-1; i >= 0; i-- )
    {
        val <<= 1;
        if( this->getValue(i) ) {
            val |= 1;
        }
    }

    return val;
}

bool HyoDioForm::getValue(int idx) const
{
    if( idx >= m_points.count() ) { return false; }

    QPushButton* pushButton = m_points.at(idx);
    if( pushButton == NULL ) { return false; }

    return pushButton->isChecked();
}

void HyoDioForm::setValue(int idx, bool val)
{
    if( idx >= m_points.count() ) { return; }

    QPushButton* pushButton = m_points.at(idx);
    if( pushButton == NULL ) { return; }

    pushButton->setChecked(val);
}

void HyoDioForm::setValue(quint64 val)
{
    quint64 bitVal = 1;
    for( int i = 0; i < m_points.count(); i++ )
    {
        setValue(i, (val&bitVal));
        bitVal <<= 1;
    }
}
