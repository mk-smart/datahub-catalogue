# MKS Data Catalogue

The MKS Data Catalogue component is a plugin for the WordPress content management system (https://wordpress.org/) which enhance it with data cataloguing capabilities, similar to popular data catalogue, registry or repository platforms. It creates a new type of posts for datasets and another one the licences (redistribution policies) associated with the datasets. It also include page templates for searching and browsing the data, as well as creating forms to register new basic datasets. 

Amongst the advantages of the MKS Data Catalogue component is the very easy deployment (as long as you have a WordPress platform installed, which is straighforward), as well as the ease of customisation and extension (by modifying the stylesheets and templates available, or creating WordPress plugins interacting with the catalogue).

## Installation 

Be sure to have:

- an instance of [WordPress](https://wordpress.org/download/release-archive/) (we recommend versions 4.3-4.7.x) configured and running;
- the [Composer](https://getcomposer.org/) PHP package manager.

Install the MKS Data Cataloguing component:

1. Clone this repository or download ZIP file and unpack it
2. Install third-party dependencies: 
2.1. Go to plugin directory: `$ cd mks-data-cataloguing`
2.2. Run `$ php composer.phar install` or `$ composer install`, depending on how you installed Composer (see its documentation)
3. Copy the `mks-data-cataloguing` directory to the `wp-content/plugins/` folder of your WordPress installation
4. Open your WordPress admin dashboard on your Web Browser, select the __Plugins__ menu and activate the __MKS Data Cataloguing__ plugin.

## How to use

Once activited, the MKS Data Cataloging plugin will create a new type of post called "Datasets" available from the right hand side menu of the WordPress admin interface. This acts like a typical WordPress post, which can be described using text, tags and categories. In addition, fields are added that are specific to datasets, including the data owner, format, status, and licences (redistribution policies).

Available licences (redistribution policies) can be edited through another type of post, which can include the text of the licence document, as well as a basic description of its clauses in terms of duties, permissions and prohibitions. 

You can also point directly to the dataset by including it as a source file or link, and attach to the dataset a number of services that use or give access to the data in a specific way.

The plugin will also create a number of page templates that can be used to create dedicated pages, including "Data Catalogue Public Page" (for searching and browsing datasets) and "New Dataset Page" (for a form to create a new dataset without using the WordPress admin interface).

## Licence and attribution

Being a WordPress plugin, MKS Data Cataloguing inherit the GPL licence from it. It is therefore available here under the GPL V3 licence (https://www.gnu.org/licenses/gpl-3.0.en.html). 

Whenever making a website that include the MKS Data Cataloguing function in a public way, please include in a visible place (e.g. you "About" page) a setence attributing the source of the plugin and linking back to this repository, e.g.

"This website makes use of the Data Cataloguing plugin developed by the Data Science Group at the Open University (http://dsg.kmi.open.ac.uk), with contributions from the MK:Smart project (http://mksmart.org) - see https://github.com/mk-smart/datahub-catalogue/"

## Compatibility with CKAN API

This is currently being developped and will be made available soon.
