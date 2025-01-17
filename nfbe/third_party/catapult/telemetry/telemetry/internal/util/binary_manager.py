# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os

from catapult_base import binary_manager
from catapult_base import dependency_util
import dependency_manager
from devil import devil_env

from telemetry.core import exceptions
from telemetry.core import util
from telemetry.core import platform as platform_module
from telemetry.internal.util import exception_formatter


TELEMETRY_PROJECT_CONFIG = os.path.join(
    util.GetTelemetryDir(), 'telemetry', 'internal', 'binary_dependencies.json')


CHROME_BINARY_CONFIG = os.path.join(util.GetCatapultDir(), 'catapult_base',
                                    'catapult_base', 'chrome_binaries.json')


NoPathFoundError = dependency_manager.NoPathFoundError
CloudStorageError = dependency_manager.CloudStorageError


_binary_manager = None


def NeedsInit():
  return not _binary_manager


def InitDependencyManager(environment_config):
  global _binary_manager
  if _binary_manager:
    raise exceptions.InitializationError(
        'Trying to re-initialize the binary manager with config %s'
        % environment_config)
  configs = [TELEMETRY_PROJECT_CONFIG, CHROME_BINARY_CONFIG]
  if environment_config:
    configs.insert(0, environment_config)
  _binary_manager = binary_manager.BinaryManager(configs)

  devil_env.config.Initialize()


def FetchPath(binary_name, arch, os_name, os_version=None):
  """ Return a path to the appropriate executable for <binary_name>, downloading
      from cloud storage if needed, or None if it cannot be found.
  """
  if _binary_manager is None:
    raise exceptions.InitializationError(
        'Called FetchPath with uninitialized binary manager.')
  return _binary_manager.FetchPath(binary_name, os_name, arch, os_version)


def LocalPath(binary_name, arch, os_name, os_version=None):
  """ Return a local path to the given binary name, or None if an executable
      cannot be found. Will not download the executable.
      """
  if _binary_manager is None:
    raise exceptions.InitializationError(
        'Called LocalPath with uninitialized binary manager.')
  return _binary_manager.LocalPath(binary_name, os_name, arch, os_version)


def FetchBinaryDepdencies(platform, client_config,
                          fetch_reference_chrome_binary):
  """ Fetch all binary dependenencies for the given |platform|.

  Note: we don't fetch browser binaries by default because the size of the
  binary is about 2Gb, and it requires cloud storage permission to
  chrome-telemetry bucket.

  Args:
    platform: an instance of telemetry.core.platform
    client_config: A path (string) to a dependencies json file.
    fetch_reference_chrome_binary: whether to fetch reference chrome binary for
      the given platform.
  """
  configs = [dependency_manager.BaseConfig(TELEMETRY_PROJECT_CONFIG)]
  dep_manager = dependency_manager.DependencyManager(configs)
  target_platform = '%s_%s' % (platform.GetOSName(), platform.GetArchName())
  dep_manager.PrefetchPaths(target_platform)

  if platform.GetOSName() == 'android':
    host_platform = '%s_%s' % (
        platform_module.GetHostPlatform().GetOSName(),
        platform_module.GetHostPlatform().GetArchName())
    dep_manager.PrefetchPaths(host_platform)

  if fetch_reference_chrome_binary:
    _FetchReferenceBrowserBinary(platform)

  # For now, handle client config separately because the BUILD.gn & .isolate of
  # telemetry tests in chromium src failed to include the files specified in its
  # client config.
  # (https://github.com/catapult-project/catapult/issues/2192)
  # For now this is ok because the client configs usually don't include cloud
  # storage infos.
  # TODO(nednguyen): remove the logic of swallowing exception once the issue is
  # fixed on Chromium side.
  if client_config:
    manager = dependency_manager.DependencyManager(
        [dependency_manager.BaseConfig(client_config)])
    try:
      manager.PrefetchPaths(target_platform)
    except Exception:
      exception_formatter.PrintFormattedException()


def _FetchReferenceBrowserBinary(platform):
  os_name = platform.GetOSName()
  arch_name = platform.GetArchName()
  manager = binary_manager.BinaryManager(
             [CHROME_BINARY_CONFIG])
  if os_name == 'android':
    os_version = dependency_util.GetChromeApkOsVersion(
        platform.GetOSVersionName())
    manager.FetchPath(
        'chrome_stable', os_name, arch_name, os_version)
  else:
    manager.FetchPath(
        'reference_build', os_name, arch_name)
