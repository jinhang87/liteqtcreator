QBS_VERSION = 1.7.1
QBS_VERSION_MAJ = $$section(QBS_VERSION, ., 0, 0)
QBS_VERSION_MIN = $$section(QBS_VERSION, ., 1, 1)
DEFINES += QBS_VERSION=\\\"$$QBS_VERSION\\\"
