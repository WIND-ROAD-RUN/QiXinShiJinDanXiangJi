#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class DlgProductScoreClass; };
QT_END_NAMESPACE

class DlgProductScore : public QDialog
{
	Q_OBJECT

public:
	DlgProductScore(QWidget* parent = nullptr);
	~DlgProductScore();

public:
	void build_ui();
	void read_config();
	void build_connect();

private slots:
	void pbtn_close_clicked();

	void rbtn_NgEnable_checked();
	void ptn_NgSimilarity_clicked();
	void ptn_NgArea_clicked();

signals:
	void scoreFormClosed();

private:
	Ui::DlgProductScoreClass* ui;
};

