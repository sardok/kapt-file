#include <apt-pkg/init.h>
#include <apt-pkg/configuration.h>
#include "apt_pkg.h"

static bool _config_init = false;

static void apt_pkg_init(void)
{
  if (_config->FindB("Initialized"))
    return;

  pkgInitConfig(*_config);
  _config->Set("Initialized", 1);
  pkgInitSystem(*_config, _system);
  _config_init = true;
}

string apt_pkg_find(const char *key)
{
  if (!_config_init)
    apt_pkg_init();

  return _config->Find(key);
}
