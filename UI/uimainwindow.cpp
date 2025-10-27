#include "uimainwindow.h"
#include "ui_uimainwindow.h"
#include <QApplication>
#include <QFile>
#include <QPushButton>

UIMainWindow::UIMainWindow(QWidget *parent)
    :QWidget(parent)
    ,ui(new Ui::UIMainWindow)
    ,m_time_frame(0)
{
    ui->setupUi(this);

    applyWpfTheme();

    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);



    m_Simulation = new RoadIntersectionSimulation(ui->graphicsView);

    connect(m_Simulation,&RoadIntersectionSimulation::updatedOneFrame,
            this,&UIMainWindow::updateStatus);
    //m_Simulation->initialize(ui->graphicsView);

    m_Demo = new RoadIntersectionSimulation(ui->m_demo_widget_1);
    m_Demo->initialize();
    m_Demo->startDemo();

    m_intro_page = new IntroPage(ui->m_first_page);

    //m_intro_page->AutoUpdate(true);

    connect(m_Demo,&RoadIntersectionSimulation::updatedOneFrame,
            m_intro_page,&IntroPage::repaintWidget);

    connect(m_intro_page,&IntroPage::PlayClicked,
            this,&UIMainWindow::onPlayButtonClicked);
    connect(m_intro_page,&IntroPage::AboutClicked,
            this,&UIMainWindow::onAboutButtonClicked);
    connect(m_intro_page,&IntroPage::HelpClicked,
            this,&UIMainWindow::onHelpButtonClicked);
    connect(m_intro_page,&IntroPage::ExitClicked,
            this,&UIMainWindow::onExitButtonClicked);

    m_setup = new SimulationSetup;

    connect(m_setup,&SimulationSetup::inputReady,
            m_Simulation,&RoadIntersectionSimulation::initializeFrominput);
    connect(m_setup,&SimulationSetup::onRandomClicked,
            m_Simulation,&RoadIntersectionSimulation::initializeFrominput);
    connect(m_setup,&SimulationSetup::onHelpClicked,
            this,&UIMainWindow::onHelpButtonClicked);

//    ui->m_visualize_panel->setController(m_Simulation->Scene()->getController());
//    connect(m_Simulation,&RoadIntersectionSimulation::updatedOneFrame,
//            ui->m_visualize_panel,&VisualizePanel::update_all);

    initializeNavigation();
}

UIMainWindow::~UIMainWindow()
{
    delete ui;
    delete m_Simulation;
    delete m_Demo;
    delete m_intro_page;
}

void UIMainWindow::onExitButtonClicked()
{
    QApplication::exit();
}

void UIMainWindow::onAboutButtonClicked()
{
    updateNavigationState(ui->m_nav_about_button);
    setPage(1,
            tr("About"),
            tr("Learn about the team, motivations, and technologies behind the simulator."));
    m_Demo->pauseSimulation();
}

void UIMainWindow::onPlayButtonClicked()
{
    updateNavigationState(ui->m_nav_simulation_button);
    setPage(4,
            tr("Simulation"),
            tr("Monitor live traffic flow and adjust behaviours in real time."));
    m_Demo->pauseSimulation();
    if(m_Simulation->State() == SimulationState::UNINITIALIZED){

        //EnableSimulationButton(false,false,false,false);
        //ui->m_simulation_page->setEnabled(false);
        m_setup->show();
    }
}

void UIMainWindow::onHelpButtonClicked()
{
    updateNavigationState(ui->m_nav_help_button);
    setPage(2,
            tr("Help"),
            tr("Follow the guided tour to explore the interface and controls."));
    m_Demo->pauseSimulation();
    ui->help_widget->startDemo();
}

void UIMainWindow::EnableSimulationButton(const bool &play,
                                          const bool &pause,
                                          const bool &stop,
                                          const bool &restart)
{
    ui->m_simulation_play_button->setEnabled(play);
    ui->m_simulation_pause_button->setEnabled(pause);
    ui->m_simulation_restart_button->setEnabled(restart);
    ui->m_simulation_stop_button->setEnabled(stop);
}

void UIMainWindow::updateStatus()
{
    m_time_frame++;

}


void UIMainWindow::on_m_about_back_button_clicked()
{
    showHomePage();
}

void UIMainWindow::on_m_help_back_button_clicked()
{
    showHomePage();
}

void UIMainWindow::on_m_simulation_back_icon_clicked()
{
    m_Simulation->pauseSimulation();
    showHomePage();
}

void UIMainWindow::on_m_setting_back_icon_clicked()
{
    showHomePage();
}

void UIMainWindow::on_m_simulation_play_button_clicked()
{    
    m_Simulation->startSimulation();
    ui->m_simulation_play_button->setEnabled(false);
    ui->m_simulation_pause_button->setEnabled(true);
}

void UIMainWindow::on_m_simulation_pause_button_clicked()
{
    m_Simulation->pauseSimulation();
    ui->m_simulation_play_button->setEnabled(true);
    ui->m_simulation_pause_button->setEnabled(false);
}

void UIMainWindow::on_m_simulation_restart_button_clicked()
{
    m_Simulation->stopSimulation();
    m_setup->show();
}

void UIMainWindow::on_m_simulation_stop_button_clicked()
{

    m_Simulation->stopSimulation();
}

void UIMainWindow::on_show_road_check_box_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_Simulation->showRoad();
    }else if(arg1 == Qt::Unchecked){
        m_Simulation->hideRoad();
    }
}

void UIMainWindow::on_show_detectors_check_box_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_Simulation->showDetectors();
    }else if(arg1 == Qt::Unchecked){
        m_Simulation->hideDetectors();
    }
}

void UIMainWindow::on_show_vehicles_vision_check_box_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_Simulation->showVehiclesVision();
    }else if(arg1 == Qt::Unchecked){
        m_Simulation->hideVehiclesVision();
    }
}

void UIMainWindow::on_show_traffic_light_check_box_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_Simulation->showTraffic();
    }else if(arg1 == Qt::Unchecked){
        m_Simulation->hideTraffic();
    }
}

void UIMainWindow::on_m_3_lanes_button_clicked()
{
    m_Simulation->setGenerationMethod(GENMETHOD::GEN_3);
}

void UIMainWindow::on_m_5_lanes_button_clicked()
{
    m_Simulation->setGenerationMethod(GENMETHOD::GEN_5);
}

void UIMainWindow::on_m_no_turn_button_clicked()
{
    m_Simulation->setGenerationMethod(GENMETHOD::NO_TURN);
}

void UIMainWindow::on_m_turn_only_button_clicked()
{
    m_Simulation->setGenerationMethod(GENMETHOD::ONLY_TURN);
}

void UIMainWindow::on_m_go_through_check_box_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        m_Simulation->turnOnGoThrough();
    }else if(arg1 == Qt::Unchecked){
        m_Simulation->turnOffGoThrough();
    }
}

void UIMainWindow::applyWpfTheme()
{
    QFile styleFile(QStringLiteral(":/styles/UI/styles/wpfui.qss"));
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        qApp->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
}

void UIMainWindow::initializeNavigation()
{
    m_navigationButtons = {
        ui->m_nav_home_button,
        ui->m_nav_simulation_button,
        ui->m_nav_settings_button,
        ui->m_nav_about_button,
        ui->m_nav_help_button
    };

    connect(ui->m_nav_home_button, &QPushButton::clicked, this, &UIMainWindow::showHomePage);
    connect(ui->m_nav_simulation_button, &QPushButton::clicked, this, [this]() {
        onPlayButtonClicked();
    });
    connect(ui->m_nav_settings_button, &QPushButton::clicked, this, &UIMainWindow::showSettingsPage);
    connect(ui->m_nav_about_button, &QPushButton::clicked, this, &UIMainWindow::onAboutButtonClicked);
    connect(ui->m_nav_help_button, &QPushButton::clicked, this, &UIMainWindow::onHelpButtonClicked);
    connect(ui->m_nav_exit_button, &QPushButton::clicked, this, &UIMainWindow::onExitButtonClicked);

    showHomePage();
}

void UIMainWindow::updateNavigationState(QPushButton *selectedButton)
{
    for (auto *button : m_navigationButtons) {
        if (button) {
            button->setChecked(button == selectedButton);
        }
    }
}

void UIMainWindow::setPage(int index, const QString &title, const QString &caption)
{
    ui->m_stacked_widget->setCurrentIndex(index);
    ui->m_header_title->setText(title);
    if (!caption.isEmpty()) {
        ui->m_header_caption->setText(caption);
    }
}

void UIMainWindow::showHomePage()
{
    updateNavigationState(ui->m_nav_home_button);
    setPage(0,
            tr("Home"),
            tr("Experience a fluid interface inspired by WPF UI."));
    ui->help_widget->stopDemo();
    m_Demo->startSimulation();
}

void UIMainWindow::showSettingsPage()
{
    updateNavigationState(ui->m_nav_settings_button);
    setPage(3,
            tr("Settings"),
            tr("Configure the simulation to match your scenario."));
    m_Demo->pauseSimulation();
}
