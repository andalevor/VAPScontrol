#ifndef GRAPHICSVIEW_HPP
#define GRAPHICSVIEW_HPP

#include <QGraphicsView>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = nullptr);

signals:
    void mousePosChanged(QPointF p);
    void mouseClicked(QPointF p);

protected Q_SLOTS:
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};

#endif // GRAPHICSVIEW_HPP
