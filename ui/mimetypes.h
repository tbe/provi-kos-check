#ifndef MIMETYPES_H
#define MIMETYPES_H

#include <QtCore/QStringList>
#include <QtCore/QMap>
QMap<QString,QStringList> mimeTypeExtensions;

void init_mimetypes() {
  mimeTypeExtensions["audio/adpcm"] << "adp"; 
  mimeTypeExtensions["audio/amr"] << "amr"; 
  mimeTypeExtensions["audio/amr-wb"] << "awb"; 
  mimeTypeExtensions["audio/annodex"] << "axa"; 
  mimeTypeExtensions["audio/basic"] << "au" << "snd"; 
  mimeTypeExtensions["audio/csound"] << "csd" << "orc" << "sco"; 
  mimeTypeExtensions["audio/flac"] << "flac"; 
  mimeTypeExtensions["audio/midi"] << "kar" << "mid" << "midi" << "rmi"; 
  mimeTypeExtensions["audio/mp4"] << "mp4a"; 
  mimeTypeExtensions["audio/mpeg"] << "m2a" << "m3a" << "m4a" << "mp2" << "mp2a" << "mp3" << "mpega" << "mpga"; 
  mimeTypeExtensions["audio/mpegurl"] << "m3u"; 
  mimeTypeExtensions["audio/ogg"] << "oga" << "ogg" << "opus" << "spx"; 
  mimeTypeExtensions["audio/prs.sid"] << "sid"; 
  mimeTypeExtensions["audio/s3m"] << "s3m"; 
  mimeTypeExtensions["audio/silk"] << "sil"; 
  mimeTypeExtensions["audio/vnd.dece.audio"] << "uva" << "uvva"; 
  mimeTypeExtensions["audio/vnd.digital-winds"] << "eol"; 
  mimeTypeExtensions["audio/vnd.dra"] << "dra"; 
  mimeTypeExtensions["audio/vnd.dts"] << "dts"; 
  mimeTypeExtensions["audio/vnd.dts.hd"] << "dtshd"; 
  mimeTypeExtensions["audio/vnd.lucent.voice"] << "lvp"; 
  mimeTypeExtensions["audio/vnd.ms-playready.media.pya"] << "pya"; 
  mimeTypeExtensions["audio/vnd.nuera.ecelp4800"] << "ecelp4800"; 
  mimeTypeExtensions["audio/vnd.nuera.ecelp7470"] << "ecelp7470"; 
  mimeTypeExtensions["audio/vnd.nuera.ecelp9600"] << "ecelp9600"; 
  mimeTypeExtensions["audio/vnd.rip"] << "rip"; 
  mimeTypeExtensions["audio/webm"] << "weba"; 
  mimeTypeExtensions["audio/x-aac"] << "aac"; 
  mimeTypeExtensions["audio/x-aiff"] << "aif" << "aifc" << "aiff"; 
  mimeTypeExtensions["audio/x-caf"] << "caf"; 
  mimeTypeExtensions["audio/x-flac"] << "flac"; 
  mimeTypeExtensions["audio/x-gsm"] << "gsm"; 
  mimeTypeExtensions["audio/x-matroska"] << "mka"; 
  mimeTypeExtensions["audio/x-mpegurl"] << "m3u"; 
  mimeTypeExtensions["audio/x-ms-wax"] << "wax"; 
  mimeTypeExtensions["audio/x-ms-wma"] << "wma"; 
  mimeTypeExtensions["audio/x-ogg"] << "oga" << "ogg" << "opus" << "spx"; 
  mimeTypeExtensions["audio/x-pn-realaudio"] << "ra" << "ram" << "rm"; 
  mimeTypeExtensions["audio/x-pn-realaudio-plugin"] << "rmp"; 
  mimeTypeExtensions["audio/x-realaudio"] << "ra"; 
  mimeTypeExtensions["audio/x-scpls"] << "pls"; 
  mimeTypeExtensions["audio/x-sd2"] << "sd2"; 
  mimeTypeExtensions["audio/x-wav"] << "wav"; 
  mimeTypeExtensions["audio/xm"] << "xm"; 
}
#endif