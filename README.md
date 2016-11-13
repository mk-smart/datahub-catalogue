# MKS Data Catalogue

The MKS Data Catalogue component is a plugin for the Wordpress content management system which enhance it with data cataloguing capabilities, similar to popular data catalogue, registry or repository platforms. It creates a new type of posts for datasets and another one the licences (redistribution policies) associated with the datasets. It also include page templates for searching and browsing the data, as well as creating forms to register new basic datasets. 

Amongst the advantages of the MKS Data Catalogue component is the very easy deployment (as long as you have a Wordpress platform installed, which is very straighforward), as well as the ease of customisation and extension (by modifying the stylesheets and templates available, or creating wordpress plugins interacting with the catalogue).

## Installation 

You should have Wordpress running and configure before installing the MKS Data Cataloguing component. Once that's done, download or clone this repository and copy the "mks-data-cataloguing" repository in the "wp-content/plugin/" folder of your wordpress installation. The plugin wil then appear in the plugin area of the Wordpress admin interface, and you should be able to activate it.

## How to use

Once activited, the MKS Data Cataloging plugin will create a new type of post called "Datasets" available from the right hand side meny of the Wordpress admin interface. This acts like a typical Wordpress posts, which can be described using text, tags and categories. In addition, fields are added that are specific to datasets, including the data owner, format, status, and licences (redistribution policies).

Available licences (redistribution policies) can be edited through another type of post, which can include the text of the licence document, as well as a basic description of its clauses in terms of duties, permissions and prohibitions. 

You can also point directly to the dataset by including it as a source file or link, and attach to the dataset a number of services that use or give access to the data in a specific way.

## Licence and attribution

Being a Wordpress plugin, MKS Data Cataloguing inherit the GPL licence from it. It is therefore available here under the GPL v3 licence. 

Whenever making a website that include the MKS Data Cataloguing function in a public way, please include in a visible place (e.g. you "About" page) a setence attributing the source of the plugin and linking back to this repository, e.g.

"This website makes use of the Data Cataloguing plugin developed by the Data Science Group at the Open University (dsg.kmi.open.ac.uk), with contributions from the MK:Smart project (mksmart.org) - see https://github.com/mk-smart/datahub-catalogue/"

## Compatibility with CKAN API

This is currently being developped and will be made available soon.
