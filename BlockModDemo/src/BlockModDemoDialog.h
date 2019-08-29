#ifndef BLOCKMODEDEMODIALOG_H
#define BLOCKMODEDEMODIALOG_H

#include <QDialog>

namespace Ui {
class BlockModDemoDialog;
}

namespace BLOCKMOD {
	class SceneManager;
}

class BlockModDemoDialog : public QDialog {
	Q_OBJECT

public:
	explicit BlockModDemoDialog(QWidget *parent = nullptr);
	~BlockModDemoDialog();

private slots:
	void on_toolButtonNew_clicked();

	void on_toolButtonOpen_clicked();

	void on_toolButtonSave_clicked();

private:
	void loadNetwork(const QString & fname);

	Ui::BlockModDemoDialog	*ui;

	BLOCKMOD::SceneManager	*m_sceneManager;
};

#endif // BLOCKMODEDEMODIALOG_H
