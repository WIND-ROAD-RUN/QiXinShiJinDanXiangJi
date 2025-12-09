#include <QMessageBox>
#include <QtWidgets/QApplication>

#include "Modules.hpp"
#include "QiXinShiJinDanXiangJi.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QiXinShiJinDanXiangJi w;

	if (!Modules::check())
	{
		return 1;
	}

#ifdef NDEBUG
	w.showFullScreen();
#else
	w.show();
#endif
    return a.exec();
}
