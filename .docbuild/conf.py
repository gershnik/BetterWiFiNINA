# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

#import sys

project = 'BetterWiFiNINA'
copyright = '2024, Eugene Gershnik'
author = 'Eugene Gershnik'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe', 'myst_parser']

templates_path = ['_templates']
exclude_patterns = []

#cpp_index_common_prefix = ['WiFi']

breathe_projects = {"BetterWiFiNINA": "../build/doxygen/xml/"}
breathe_default_project = "BetterWiFiNINA"


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinxdoc'
html_theme_options = {'sidebarwidth':  400}
html_title = 'BetterWiFiNINA'

html_static_path = ['_static']
