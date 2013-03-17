#ifndef _DIST_H
#define _DIST_H
#include <QVariant>
#include <QString>
#include <QList>
typedef enum {
  DIST_SOURCE_INVALID = 0,
  DIST_SOURCE_HTTP,
  DIST_SOURCE_CDROM,
  DIST_SOURCE_SSH,
  DIST_SOURCE_RSH,
} dist_source_t;

using std::string;

class Dist
{
public:
  Dist(Dist *parent = NULL);
  ~Dist();
  void set_name(const QString &name);
  const QString &get_name(void) const;
  void set_dist_type(const QString &type);
  void set_dist_type(const string &type);
  const QString &get_dist_type(void);
  void set_arch(const QString &arch);
  void set_arch(const string &arch);
  const QString &get_arch() const;
  bool get_cached() const;
  void SetMajor(bool set);
  bool is_major() const;

  bool Search(const QString &name);
  void AddEntry(const QString &host,
      const QString &path, dist_source_t proto);
  static void Merge(Dist *dst, const Dist *src);
  bool IsChecked() const;
  void SetChecked(bool set);
    
  /* Functions to be used in tree model. */
  void appendChild(Dist *child);
  Dist *child(int row);
  int childCount() const;
  int columnCount() const;
  int row() const;
  Dist *parent();
  void cache();

    
protected:
  typedef struct
  {
    bool cached;
    dist_source_t source_type;
    QString host;			/* Base address. */
    QString path;
    QString local_content_file;
    QString remote_content_file;
  } source_entry_t;
  source_entry_t *_entry;
  QList<Dist *> children;

private:
  QString GetSourceProto(dist_source_t src) const;
  static void MergeEntries(QList<source_entry_t> *dst_entries,
      const QList<source_entry_t> *src_entries);
  QString get_cache_dir();
  void do_cache(source_entry_t *entry);
  void cache(Dist *dist);
  QString _name;
  QString _type; /* Debian?, Ubuntu? */
  Dist *_parent;			/* Pointer to parent of the dist. */
  bool _major;			/* True if dist is the main source (not security or backport). */
  bool _enabled;			/* Check state on UI */
  QString _arch;

};
#endif
