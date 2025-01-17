# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os

from telemetry.page import action_runner
from telemetry.testing import serially_executed_browser_test_case


def ConvertPathToTestName(url):
  return url.replace('.', '_')


class SimpleBrowserTest(
    serially_executed_browser_test_case.SeriallyBrowserTestCase):

  @classmethod
  def GenerateTestCases_TestJavascript(cls, options):
    del options  # unused
    for path in ['page_with_link.html', 'page_with_clickables.html']:
      yield 'add_1_and_2_' + ConvertPathToTestName(path), (path, 1, 2, 3)

  @classmethod
  def setUpClass(cls):
    super(cls, SimpleBrowserTest).setUpClass()
    cls.action_runner = action_runner.ActionRunner(cls._browser.tabs[0])
    cls.SetStaticServerDir(
        os.path.join(os.path.abspath(__file__), '..', 'pages'))

  def TestJavascript(self, file_path, num_1, num_2, expected_sum):
    url = self.UrlOfStaticFilePath(file_path)
    self.action_runner.Navigate(url)
    actual_sum = self.action_runner.EvaluateJavaScript(
        '%i + %i' % (num_1, num_2))
    self.assertEquals(expected_sum, actual_sum)

  def testClickablePage(self):
    url = self.UrlOfStaticFilePath('page_with_clickables.html')
    self.action_runner.Navigate(url)
    self.action_runner.ExecuteJavaScript('valueSettableByTest = 1997')
    self.action_runner.ClickElement(text='Click/tap me')
    self.assertEqual(1997, self.action_runner.EvaluateJavaScript('valueToTest'))

  def testAndroidUI(self):
    if self._platform.GetOSName() != 'android':
      self.skipTest('The test is for android only')
    url = self.UrlOfStaticFilePath('page_with_clickables.html')
    # Nativgate to page_with_clickables.html
    self.action_runner.Navigate(url)
    # Click on history
    self._platform.system_ui.WaitForUiNode(
        resource_id='com.android.chrome:id/menu_button')
    self._platform.system_ui.GetUiNode(
        resource_id='com.android.chrome:id/menu_button').Tap()
    self._platform.system_ui.WaitForUiNode(content_desc='History')
    self._platform.system_ui.GetUiNode(content_desc='History').Tap()
    # Click on the first entry of the history (page_with_clickables.html)
    self.action_runner.WaitForElement('#id-0')
    self.action_runner.ClickElement('#id-0')
    # Verify that the page's js is interactable
    self.action_runner.WaitForElement(text='Click/tap me')
    self.action_runner.ExecuteJavaScript('valueSettableByTest = 1997')
    self.action_runner.ClickElement(text='Click/tap me')
    self.assertEqual(1997, self.action_runner.EvaluateJavaScript('valueToTest'))
