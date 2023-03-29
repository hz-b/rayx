# How to use RayCore

--- This page is outdated ---

On this page you will find out how to use the RayCore library to build an Application.

RayCore defines an EntryPoint in the file: EntryPoint.h. As you can see it expects an external Application-subclass to define the CreateApplication()-function that returns a pointer to an Application object. In this function you can start threads, define configurations, start a gui and so on.