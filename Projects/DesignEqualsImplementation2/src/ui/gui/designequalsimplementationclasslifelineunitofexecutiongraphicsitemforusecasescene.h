#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H

#include <QGraphicsRectItem>

class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene;
class DesignEqualsImplementationClassLifeLineUnitOfExecution;
class IDesignEqualsImplementationHaveOrderedListOfStatements;
class IDesignEqualsImplementationStatement;

typedef QPair<QGraphicsLineItem* /*visual representation*/, IDesignEqualsImplementationStatement* /*underlyingStatement*/> ExistingStatementListEntryTypedef;

class DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, const QRectF &rect, QGraphicsItem *parent = 0);
    explicit DesignEqualsImplementationClassLifeLineUnitOfExecutionGraphicsItemForUseCaseScene(DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution, DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    int getInsertIndexForMouseScenePos(QPointF mouseEventScenePos);
    QGraphicsItem *makeSnappingHelperForMousePoint(QPointF mouseScenePos);

    DesignEqualsImplementationClassLifeLineUnitOfExecution *unitOfExecution() const;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *parentClassLifeline() const;
    virtual int type() const;
private:
    DesignEqualsImplementationClassLifeLineUnitOfExecution *m_UnitOfExecution;
    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *m_ParentClassLifeline;
    QList<qreal> m_VerticalPositionsOfSnapPoints;
    QList<ExistingStatementListEntryTypedef> m_ExistingStatements;

    static const QRectF minRect();
    void privateConstructor();
    void insertStatement(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
    void repositionExistingStatementsAndSnapPoints();
private slots:
    void handleStatementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSLIFELINEUNITOFEXECUTIONGRAPHICSITEMFORUSECASESCENE_H
