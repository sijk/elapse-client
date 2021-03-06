/*
 * Elapse Client
 * Copyright (C) 2015 Simon Knopp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7:
 *
 * The licensors of this Program grant you additional permission to link
 * this Program with other modules by means of the provided plugin
 * interface and to convey the resulting work, regardless of the license
 * terms of the other modules.
 */

#include <QMap>
#include <QBrush>
#include "logmodel.h"

namespace elapse { namespace log {

static const QMap<QxtLogger::LogLevel, QBrush> levelColours = {
    { QxtLogger::TraceLevel,    Qt::gray        },
    { QxtLogger::DebugLevel,    Qt::lightGray   },
    { QxtLogger::InfoLevel,     Qt::white       },
    { QxtLogger::WarningLevel,  Qt::yellow      },
    { QxtLogger::ErrorLevel,    Qt::red         },
    { QxtLogger::CriticalLevel, Qt::magenta     },
    { QxtLogger::FatalLevel,    Qt::darkMagenta },
    { QxtLogger::WriteLevel,    Qt::white       },
};

static const QMap<QxtLogger::LogLevel, QString> levelNames = {
    { QxtLogger::TraceLevel,    QStringLiteral("Trace")    },
    { QxtLogger::DebugLevel,    QStringLiteral("Debug")    },
    { QxtLogger::InfoLevel,     QStringLiteral("Info")     },
    { QxtLogger::WarningLevel,  QStringLiteral("Warning")  },
    { QxtLogger::ErrorLevel,    QStringLiteral("Error")    },
    { QxtLogger::CriticalLevel, QStringLiteral("Critical") },
    { QxtLogger::FatalLevel,    QStringLiteral("Fatal")    },
    { QxtLogger::WriteLevel,    QStringLiteral("Write")    },
};

/*!
 * Construct a new LogModel as a child of the given \a parent.
 */
LogModel::LogModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _items.size();
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > _items.size())
        return QVariant();

    const LogItem &item = _items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return item.time;
        case 1:
            return levelNames.value(item.level);
        case 2:
            return item.message;
        }

    case LOG_LEVEL_ROLE:
        return item.level;

    case Qt::BackgroundRole:
        switch (index.column()) {
        case 1:
            return levelColours.value(item.level);
        }

    case Qt::TextAlignmentRole:
        switch (index.column()) {
        case 1:
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case 0:
        return "Time";
    case 1:
        return "Level";
    case 2:
        return "Message";
    }

    return QVariant();
}

/*!
 * Append the given \a item to the table model.
 */
void LogModel::appendItem(const LogItem &item)
{
    int row = _items.size();
    beginInsertRows(QModelIndex(), row, row);
    _items.append(item);
    endInsertRows();
}

}} // namespace elapse::log
