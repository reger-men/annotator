#include "pluginswidget.h"
#include "ui_pluginswidget.h"
#include "plugins/plugin.h"
#include "plugins/pluginloader.h"

PluginsWidget::PluginsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginsWidget)
{
    ui->setupUi(this);

    reload();
}

PluginsWidget::~PluginsWidget()
{
    delete ui;
}

void PluginsWidget::reload()
{
    ui->comboBox->clear();
    for(Annotator::Plugin *plugin: Annotator::PluginLoader::getInstance().getPlugins().toStdList())
    {
        ui->comboBox->addItem(plugin->getName());
    }
    on_comboBox_currentIndexChanged(ui->comboBox->currentText());
    this->updateGeometry();
    ui->gridLayout->update();
    ui->scrollArea->updateGeometry();
}

void PluginsWidget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    Annotator::PluginLoader::getInstance().setCurrent(arg1);
    Annotator::Plugin * plugin = Annotator::PluginLoader::getInstance().getPlugin(arg1);

    if(plugin){
        if(lastWidget != nullptr){
            ui->layout->takeAt(0);
            lastWidget->setParent(nullptr);
        }
        lastWidget = plugin->getWidget();
        ui->layout->addWidget(lastWidget);
    }
}
