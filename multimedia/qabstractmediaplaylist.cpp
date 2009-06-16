#include "qabstractmediaplaylist.h"
#include "qabstractmediaplaylist_p.h"

/*!
    \class QAbstractMediaPlaylist
    \ingroup multimedia

    \preliminary

    \brief The QAbstractMediaPlaylist class is the abstract base class
    of media playlists, providing functionality common to playlists.

    \sa QMediaSource
*/


/*!
    \fn QAbstractMediaPlaylist::currentItemPos()
*/

/*!
    \fn QAbstractMediaPlaylist::size()
 */

/*!
    \fn QAbstractMediaPlaylist::operator [](int pos)
 */

/*!
    \fn QAbstractMediaPlaylist::append(const QMediaSource &source)
*/

/*!
    \fn QAbstractMediaPlaylist::append(const QList<QMediaSource> &sources)
*/

/*!
    \fn QAbstractMediaPlaylist::insert(int pos, const QMediaSource &source)
*/

/*!
    \fn QAbstractMediaPlaylist::remove(int start, int end)
*/

/*!
    \fn QAbstractMediaPlaylist::isShuffled()
*/

/*!
    \fn QAbstractMediaPlaylist::jump(int)
*/

/*!
    \fn QAbstractMediaPlaylist::shuffle()
*/

/*!
    \fn QAbstractMediaPlaylist::unshuffle()
*/

/*!
  */
QAbstractMediaPlaylist::QAbstractMediaPlaylist(QObject* parent)
    :QObject(*new QAbstractMediaPlaylistPrivate, parent)
{
}

QAbstractMediaPlaylist::QAbstractMediaPlaylist(QAbstractMediaPlaylistPrivate &dd, QObject *parent)
    :QObject(dd,parent)
{
}

/*!
  */
QAbstractMediaPlaylist::~QAbstractMediaPlaylist()
{
}

/*!
  */
bool QAbstractMediaPlaylist::load(const QString &location, const char *format)
{
    return false;
}

/*!
  */
bool QAbstractMediaPlaylist::save(const QString &location, const char *format)
{
    return false;
}

/*!
  */
QMediaSource QAbstractMediaPlaylist::currentItem() const
{
    return (*this)[currentItemPos()];
}

/*!
  */
QMediaSource QAbstractMediaPlaylist::nextItem() const
{
    int nextPos = currentItemPos()+1;
    if (nextPos < size())
        return  (*this)[currentItemPos()+1];
    else
        return QMediaSource();
}

/*!
  */
bool QAbstractMediaPlaylist::isEmpty() const
{
    return size()==0;
}

/*!
  */
void QAbstractMediaPlaylist::remove(int pos)
{
    remove(pos,pos);
}

/*!
  */
void QAbstractMediaPlaylist::clear()
{
    remove(0,size()-1);
}

/*!
  */
void QAbstractMediaPlaylist::next()
{
    int nextPos = currentItemPos()+1;
    if (nextPos < size())
        jump(nextPos);
}

/*!
  */
void QAbstractMediaPlaylist::prev()
{
    int prevPos = currentItemPos()-1;
    if (prevPos >= 0)
        jump(prevPos);
}
