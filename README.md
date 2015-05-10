# coprissi

## Overview

Coprissi is an image management tool. It recursively scans for image files in a source directory, creates an output directory structure based on the dates the photos were taken, and copies the images into the appropriate output folders.

## Usage

    $ coprissi <source> <destination>

## Example

Suppose you have a source directory with these images:

    src/Josie's Bday 01.jpg
    src/Josie's Bday 02.jpg
    src/Josie's Bday 03.jpg
    src/Mike's Bday 01.jpg
    src/Mike's Bday 02.jpg
    src/Mike's Bday 03.jpg
    src/Mike's Bday 04.jpg
    src/Mike's Bday 05.jpg
    src/Mike's Bday 06.jpg
    src/Hockey 01.jpg
    src/Hockey 02.jpg
    src/Hockey 03.jpg

Coprissi will scan the directory and output the following:

    out/1998/11/03/Mike's Bday 01.jpg
    out/1998/11/03/Mike's Bday 02.jpg
    out/1998/11/03/Mike's Bday 03.jpg
    out/1998/11/03/Mike's Bday 04.jpg
    out/1998/11/03/Mike's Bday 05.jpg
    out/1998/11/03/Mike's Bday 06.jpg
    out/2000/04/05/Josie's Bday 01.jpg
    out/2000/04/05/Josie's Bday 02.jpg
    out/2000/04/05/Josie's Bday 03.jpg
    out/2014/05/06/Hockey 01.jpg
    out/2014/05/06/Hockey 02.jpg
    out/2014/05/06/Hockey 03.jpg
