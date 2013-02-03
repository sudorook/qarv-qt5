/*
    qarv, a Qt interface to aravis.
    Copyright (C) 2012  Jure Varlec <jure.varlec@ad-vega.si>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "api/qarvgui.h"
#include "qarvmainwindow.h"
#include "globals.h"

#include <QTranslator>

using namespace QArv;

class QArvGuiExtension {
public:
  QArvMainWindow* mw;
};

/*! Translations and event filters are loaded.
 */
void QArvGui::init(QApplication* a) {
  auto trans = new QTranslator(a);
  auto locale = QLocale::system().name();
  if (trans->load(QString("qarv_") + locale, qarv_datafiles)) {
    a->installTranslator(trans);
  } else {
    delete trans;
  }

  // Install a global event filter that makes sure that long tooltips can be word-wrapped
  const int tooltip_wrap_threshold = 70;
  a->installEventFilter(new ToolTipToRichTextFilter(tooltip_wrap_threshold, a));
}

/*! In standalone mode, the GUI presents full recording facilities. When not in
 * standalone mode, only the record button is available. When toggled, frames
 * will be decoded and made available using getFrame().
 */
QArvGui::QArvGui(QWidget* parent, bool standalone) :
  QObject(parent), mainWindowAlive(true) {

  ext = new QArvGuiExtension;
  ext->mw = new QArvMainWindow(parent, standalone);
  thewidget = ext->mw;
  connect(ext->mw, SIGNAL(recordingStarted(bool)), SLOT(signalForwarding(bool)));
  connect(ext->mw, SIGNAL(destroyed(QObject*)), SLOT(mainWindowDestroyed(QObject*)));
}

QArvGui::~QArvGui() {
  delete ext;
  if (mainWindowAlive) {
    thewidget->close();
    delete thewidget;
  }
}

void QArvGui::mainWindowDestroyed(QObject*) {
  mainWindowAlive = false;
}

QWidget* QArvGui::widget() {
  return thewidget;
}

void QArvGui::signalForwarding(bool enable) {
  if (enable)
    connect(ext->mw->camera, SIGNAL(frameReady()), SIGNAL(frameReady()));
  else
    disconnect(ext->mw->camera, SIGNAL(frameReady()), this, SIGNAL(frameReady()));
}

/*!
 * \param processed The frame as seen in the GUI video display.
 * \param unprocessed The decoded frame with no transformations applied.
 * \param raw Undecoded buffer.
 * \param rawAravisBuffer See QArvCamera::getFrame().
 * \param nocopy See QArvCamera::getFrame().
 */
void QArvGui::getFrame(QImage* processed,
                       QImage* unprocessed,
                       QByteArray* raw,
                       ArvBuffer** rawAravisBuffer,
                       bool nocopy) {
  ext->mw->getNextFrame(processed, unprocessed, raw, rawAravisBuffer, nocopy);
}

void QArvGui::getFrame(QImage* processed,
                       QImage* unprocessed,
                       QByteArray* raw,
                       ArvBuffer** rawAravisBuffer) {
  getFrame(processed, unprocessed, raw, rawAravisBuffer,
           ext->mw->nocopyCheck->isChecked());
}

/*! This function only works when not in standalone mode. It is useful when the
 * caller relies on fixed frame format and wants to disallow changing it during
 * operation.
 */
void QArvGui::forceRecording() {
  if (!ext->mw->standalone) {
    if (!ext->mw->recordButton->isChecked()) ext->mw->recordButton->click();
    ext->mw->recordButton->setEnabled(false);
  }
}

/*! While the camera shouldn't normally be used outside the GUI, it is
 * sometimes useful to be able to get information about it, such as the
 * current pixel format.
 */
QArvCamera* QArvGui::camera() {
  return ext->mw->camera;
}