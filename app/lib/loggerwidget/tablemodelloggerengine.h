#ifndef TABLEMODELLOGGERENGINE_H
#define TABLEMODELLOGGERENGINE_H

#include <memory>
#include <QxtLoggerEngine>

class QAbstractItemModel;

namespace elapse { namespace log {

class LogModel;


/*!
 * \brief The TableModelLoggerEngine class is a QxtLoggerEngine that stores
 * messages in a table model.
 * \ingroup logging
 */

class TableModelLoggerEngine : public QxtLoggerEngine
{
public:
    TableModelLoggerEngine();
    ~TableModelLoggerEngine();

    void initLoggerEngine() override;
    void killLoggerEngine() override;
    bool isInitialized() const override;
    void writeFormatted(QxtLogger::LogLevel level,
                        const QList<QVariant> &messages) override;

    QAbstractItemModel *model();

private:
    std::unique_ptr<LogModel> _model;
};

}} // namespace elapse::log

#endif // TABLEMODELLOGGERENGINE_H
