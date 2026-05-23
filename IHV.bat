rd \IHV /q /s

md \IHV\Core
md \IHV\Core\Inc
md \IHV\Core\Lib
md \IHV\Engine
md \IHV\Engine\Inc
md \IHV\Engine\Lib
md \IHV\GlideDrv
md \IHV\GlideDrv\Src
md \IHV\GlideDrv\Lib

copy Core\Inc\*.h          \IHV\Core\Inc
copy Core\Lib\*.lib        \IHV\Core\Lib
copy Engine\Inc\*.h        \IHV\Engine\Inc
copy Engine\Lib\*.lib      \IHV\Engine\Lib
copy GlideDrv\Src\*.h      \IHV\GlideDrv\Src
copy GlideDrv\Src\*.cpp    \IHV\GlideDrv\Src
copy GlideDrv\Src\*.dsp    \IHV\GlideDrv\Src
copy GlideDrv\Lib\*.lib    \IHV\GlideDrv\Lib
