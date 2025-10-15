#include <QMessageBox>
#include <QtWidgets/QApplication>
#include "rqw_RunEnvCheck.hpp"
#include "Utilty.hpp"
#include "QiXinShiJinDanXiangJi.h"
#include "QiXinShiJinDanXiangJi.hpp"
#include "oso_func.hpp"
#include "ScoreConfig.hpp"
#include "SetConfig.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QiXinShiJinDanXiangJi w;

	if (!QiXinShiJinDanXiangJi::check())
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
