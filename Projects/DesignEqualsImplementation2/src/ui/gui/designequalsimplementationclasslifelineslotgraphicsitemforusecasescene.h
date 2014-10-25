#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINESLOTGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINESLOTGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsRectItem>
#include "isnappablesourcegraphicsitem.h"

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene;
class DesignEqualsImplementationClassSlot;
class IDesignEqualsImplementationHaveOrderedListOfStatements;
class IDesignEqualsImplementationStatement;
class IRepresentSnapGraphicsItemAndProxyGraphicsItem;

typedef QPair<QGraphicsItem* /*visual representation*/, IDesignEqualsImplementationStatement* /*underlyingStatement*/> ExistingStatementListEntryTypedef;

class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem, public ISnappableSourceGraphicsItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, const QRectF &rect, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationSlotGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeLine, DesignEqualsImplementationClassSlot *slot, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
    virtual ~DesignEqualsImplementationSlotGraphicsItemForUseCaseScene() { }

    int getInsertIndexForMouseScenePos(QPointF mouseEventScenePos);
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForSlotEntryPoint(QPointF mouseScenePosForDeterminingRightOrLeftOnly);
    virtual IRepresentSnapGraphicsItemAndProxyGraphicsItem *makeSnappingHelperForMousePoint(QPointF mouseScenePos);

    //DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution() const;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifelineGraphicsItem() const;
    DesignEqualsImplementationClassSlot* underlyingSlot() const;
    virtual int type() const;

    //QPointF calculatePointForStatementsP1AtStatementIndex(int statementIndexToCalculateP1for);
    QList<ExistingStatementListEntryTypedef> existingStatementsAndTheirGraphicsItems() const;
private:
    //DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecution;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *m_ParentClassLifeline;
    DesignEqualsImplementationClassSlot *m_Slot;
    QList<qreal> m_VerticalPositionsOfSnapPoints;
    QList<ExistingStatementListEntryTypedef> m_ExistingStatementsAndTheirGraphicsItems;

    static const QRectF minRect();
    void privateConstructor();
    void insertStatementGraphicsItem(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
    void repositionExistingStatementsAndSnapPoints();
    qreal calculateMyRectHeightUsingExistingStatements();
signals:
    void geometryChanged();
private slots:
    void handleStatementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINESLOTGRAPHICSITEMFORUSECASESCENE_H
