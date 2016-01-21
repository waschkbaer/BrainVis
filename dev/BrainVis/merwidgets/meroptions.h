#ifndef MEROPTIONS_H
#define MEROPTIONS_H

#include <QDockWidget>

#include <core/Math/Vectors.h>

struct MERRecordSettings{

    MERRecordSettings():
      _targetPosition(Core::Math::Vec3f(0,0,0)),
      _entryPosition(Core::Math::Vec3f(0,0,0)),
      _centerIndex(2),
      _lateralIndex(3),
      _anteriorIndex(4),
      _posteriorIndex(-1),
      _medianIndex(-1),
      _isRightSide(true)
    {}

    Core::Math::Vec3f   _targetPosition;
    Core::Math::Vec3f   _entryPosition;

    int                 _centerIndex;
    int                 _lateralIndex;
    int                 _anteriorIndex;
    int                 _posteriorIndex;
    int                 _medianIndex;

    bool                _isRightSide;
};

namespace Ui {
class MEROptions;
}

class MEROptions : public QDockWidget
{
    Q_OBJECT

public:
    explicit MEROptions(std::shared_ptr<MERRecordSettings> settings, QWidget *parent = 0);
    ~MEROptions();

    void updateSettings();

private slots:


    void on_saveButton_clicked();

private:
    Ui::MEROptions *ui;

    std::shared_ptr<MERRecordSettings> _settings;
};

#endif // MEROPTIONS_H
