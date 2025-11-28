#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <vector>

class ChainableLabel : public QLabel {
    Q_OBJECT

signals:
    void valueChanged(int value);

public:
    explicit ChainableLabel(const QString& text, QWidget* parent = nullptr)
        : QLabel(text, parent) {}

public slots:
    void setNumAndEmit(int value) {
        setNum(value);
        emit valueChanged(value);
    }
};

class WidgetCollection : public QWidget {
    Q_OBJECT

private:
    std::vector<QWidget*> widgets;
    QVBoxLayout* widgetsLayout;
    QComboBox* typeSelector;
    std::vector<QCheckBox*> checkboxes;
    int widgetCounter = 0;

public:
    explicit WidgetCollection(QWidget* parent = nullptr);

private:
    void setupUI();
    const char* getSignalForWidget(QWidget* widget);
    const char* getSlotForWidget(QWidget* widget);
    bool isPrimarySource(QWidget* widget);
    bool canRelay(QWidget* widget);
    bool canReceive(QWidget* widget);
    bool connectWidgets(QWidget* sender, QWidget* receiver, int& connectionsCount);
    void classifyWidgets(std::vector<QWidget*>& senders, std::vector<QWidget*>& relays, std::vector<QWidget*>& receivers);
    void connectChain(const std::vector<QWidget*>& senders, const std::vector<QWidget*>& relays, const std::vector<QWidget*>& receivers, int& connectionsCount);
    void updateCheckboxes();
    void disconnectWidget(QWidget* widget);
    void addWidget();
    void connectAll();
    void debugConnections();
};
