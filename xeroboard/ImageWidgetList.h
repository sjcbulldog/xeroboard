#pragma once

#include <QListWidget>

class CustomImageMgr;

class ImageWidgetList : public QListWidget
{
public:
	ImageWidgetList(CustomImageMgr& mgr, QWidget* parent = nullptr);
	virtual ~ImageWidgetList();

	virtual QMimeData* mimeData(const QList<QListWidgetItem*> items) const;

private:
	CustomImageMgr& mgr_;
};

