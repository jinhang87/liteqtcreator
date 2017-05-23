#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

namespace Ui {
class mainmenu;
}

namespace Core {
namespace Internal {

class mainmenu : public QWidget
{
    Q_OBJECT

public:
    explicit mainmenu(QWidget *parent = 0);
    ~mainmenu();

private:
    Ui::mainmenu *ui;
};

} // namespace Internal
} // namespace Core

#endif // MAINMENU_H
