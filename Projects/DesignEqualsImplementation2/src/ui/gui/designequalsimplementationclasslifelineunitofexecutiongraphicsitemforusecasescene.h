#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsRectItem>

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene;
class DesignEqualsImplementationClassSlot;
class IDesignEqualsImplementationHaveOrderedListOfStatements;
class IDesignEqualsImplementationStatement;

typedef QPair<QGraphicsLineItem* /*visual representation*/, IDesignEqualsImplementationStatement* /*underlyingStatement*/> ExistingStatementListEntryTypedef;

class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, const QRectF &rect, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    int getInsertIndexForMouseScenePos(QPointF mouseEventScenePos);
    QGraphicsItem *makeSnappingHelperForMousePoint(QPointF mouseScenePos);

    //DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution() const;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifelineGraphicsItem() const;
    DesignEqualsImplementationClassSlot* underlyingSlot() const;
    virtual int type() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecution;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *m_ParentClassLifeline;
    DesignEqualsImplementationClassSlot *m_Slot;
    QList<qreal> m_VerticalPositionsOfSnapPoints;
    QList<ExistingStatementListEntryTypedef> m_ExistingStatements;

    static const QRectF minRect();
    void privateConstructor();
    void insertStatementGraphicsItem(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
    void repositionExistingStatementsAndSnapPoints();
signals:
    void geometryChanged();
private slots:
    void handleStatementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
