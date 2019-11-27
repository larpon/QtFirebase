#ifndef QTFIREBASE_INSTANCE_H
#define QTFIREBASE_INSTANCE_H

class QtFirebaseAdMob;

#if defined(qFirebaseAdMob)
#undef qFirebaseAdMob
#endif
#define qFirebaseAdMob (static_cast<QtFirebaseAdMob *>(QtFirebaseAdMob::instance()))

#endif // QTFIREBASE_INSTANCE_H
