#ifndef STRIPCHART_H
#define STRIPCHART_H

#include <QWidget>
#include <QScopedPointer>

namespace elapse { namespace widgets {

class StripChartPrivate;

/*!
 * \brief The StripChart class provides a generic stripchart widget for
 * displaying time-series data.
 *
 * Redrawing is handled intelligently. If calls to appendData() occur less than
 * \c rate milliseconds apart, the plot will be drawn every \c rate
 * milliseconds. If appendData() calls occur more than \c rate milliseconds
 * apart, the plot will be redrawn on each appendData() call. If appendData()
 * is not called, the plot is not redrawn. This approach minimises computation
 * while ensuring display responsiveness.
 *
 * \ingroup widgets
 */

class StripChart : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(uint nStrips READ nStrips WRITE setNStrips)
    Q_PROPERTY(uint nSamples READ nSamples WRITE setNSamples)
    Q_PROPERTY(double stripSpacing READ spacing WRITE setSpacing)
    Q_PROPERTY(uint rate READ rate)

public:
    explicit StripChart(QWidget *parent = nullptr);
    ~StripChart();

public slots:
    void appendData(const std::vector<double> &data);

    uint nStrips() const;
    void setNStrips(uint n);

    uint nSamples() const;
    void setNSamples(uint n);

    double spacing() const;
    void setSpacing(double spacing);
    void setSpacing(int spacing) { setSpacing(double(spacing)); }

    uint rate() const;

private:
    const QScopedPointer<StripChartPrivate> d_ptr;
    Q_DECLARE_PRIVATE(StripChart)
};

}} // namespace elapse::widgets

#endif // STRIPCHART_H
