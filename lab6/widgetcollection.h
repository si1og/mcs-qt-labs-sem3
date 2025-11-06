#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <vector>

class WidgetCollection : public QWidget {
    Q_OBJECT

private:
    std::vector<QWidget*> widgets;
    QVBoxLayout* widgetsLayout;
    QComboBox* typeSelector;
    int widgetCounter = 0;

public:
    explicit WidgetCollection(QWidget* parent = nullptr);

private:
    void setupUI();
    const char* getSignalForWidget(QWidget* widget);
    const char* getSlotForWidget(QWidget* widget);
    void addWidget();
    void connectAll();
    void debugConnections();
};
