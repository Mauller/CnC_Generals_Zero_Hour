set(GAMEENGINEDEVICE_SRC
#    Include/MilesAudioDevice/MilesAudioManager.h
    Include/VideoDevice/Bink/BinkVideoPlayer.h
    Include/W3DDevice/Common/W3DConvert.h
    Include/W3DDevice/Common/W3DFunctionLexicon.h
    Include/W3DDevice/Common/W3DModuleFactory.h
    Include/W3DDevice/Common/W3DRadar.h
    Include/W3DDevice/Common/W3DThingFactory.h
    Include/W3DDevice/GameClient/HeightMap.h
    Include/W3DDevice/GameClient/Module/W3DDebrisDraw.h
    Include/W3DDevice/GameClient/Module/W3DDefaultDraw.h
    Include/W3DDevice/GameClient/Module/W3DDependencyModelDraw.h
    Include/W3DDevice/GameClient/Module/W3DLaserDraw.h
    Include/W3DDevice/GameClient/Module/W3DModelDraw.h
    Include/W3DDevice/GameClient/Module/W3DOverlordTankDraw.h
    Include/W3DDevice/GameClient/Module/W3DPoliceCarDraw.h
    Include/W3DDevice/GameClient/Module/W3DProjectileStreamDraw.h
    Include/W3DDevice/GameClient/Module/W3DRopeDraw.h
    Include/W3DDevice/GameClient/Module/W3DScienceModelDraw.h
    Include/W3DDevice/GameClient/Module/W3DSupplyDraw.h
    Include/W3DDevice/GameClient/Module/W3DTankDraw.h
    Include/W3DDevice/GameClient/Module/W3DTankTruckDraw.h
    Include/W3DDevice/GameClient/Module/W3DTracerDraw.h
    Include/W3DDevice/GameClient/Module/W3DTruckDraw.h
    Include/W3DDevice/GameClient/TerrainTex.h
    Include/W3DDevice/GameClient/TileData.h
    Include/W3DDevice/GameClient/W3DAssetManager.h
    Include/W3DDevice/GameClient/W3DAssetManagerExposed.h
    Include/W3DDevice/GameClient/W3DBibBuffer.h
    Include/W3DDevice/GameClient/W3DBridgeBuffer.h
    Include/W3DDevice/GameClient/W3DBufferManager.h
    Include/W3DDevice/GameClient/W3DCustomEdging.h
    Include/W3DDevice/GameClient/W3DCustomScene.h
    Include/W3DDevice/GameClient/W3DDebugDisplay.h
    Include/W3DDevice/GameClient/W3DDebugIcons.h
    Include/W3DDevice/GameClient/W3DDisplay.h
    Include/W3DDevice/GameClient/W3DDisplayString.h
    Include/W3DDevice/GameClient/W3DDisplayStringManager.h
    Include/W3DDevice/GameClient/W3DDynamicLight.h
    Include/W3DDevice/GameClient/W3DFileSystem.h
    Include/W3DDevice/GameClient/W3DGadget.h
    Include/W3DDevice/GameClient/W3DGameClient.h
    Include/W3DDevice/GameClient/W3DGameFont.h
    Include/W3DDevice/GameClient/W3DGameWindow.h
    Include/W3DDevice/GameClient/W3DGameWindowManager.h
    Include/W3DDevice/GameClient/W3DGUICallbacks.h
    Include/W3DDevice/GameClient/W3DInGameUI.h
    Include/W3DDevice/GameClient/W3DMirror.h
    Include/W3DDevice/GameClient/W3DMouse.h
    Include/W3DDevice/GameClient/W3DParticleSys.h
    Include/W3DDevice/GameClient/W3DPoly.h
    Include/W3DDevice/GameClient/W3DProjectedShadow.h
    Include/W3DDevice/GameClient/W3DRoadBuffer.h
    Include/W3DDevice/GameClient/W3DScene.h
    Include/W3DDevice/GameClient/W3DShaderManager.h
    Include/W3DDevice/GameClient/W3DShadow.h
    Include/W3DDevice/GameClient/W3DShroud.h
    Include/W3DDevice/GameClient/W3DStatusCircle.h
    Include/W3DDevice/GameClient/W3DTerrainTracks.h
    Include/W3DDevice/GameClient/W3DTerrainVisual.h
    Include/W3DDevice/GameClient/W3DTreeBuffer.h
    Include/W3DDevice/GameClient/W3DVideobuffer.h
    Include/W3DDevice/GameClient/W3DView.h
    Include/W3DDevice/GameClient/W3DVolumetricShadow.h
    Include/W3DDevice/GameClient/W3DWater.h
    Include/W3DDevice/GameClient/W3DWaterTracks.h
    Include/W3DDevice/GameClient/W3DWaypointBuffer.h
    Include/W3DDevice/GameClient/W3DWebBrowser.h
    Include/W3DDevice/GameClient/WorldHeightMap.h
    Include/W3DDevice/GameLogic/W3DGameLogic.h
    Include/W3DDevice/GameLogic/W3DGhostObject.h
    Include/W3DDevice/GameLogic/W3DTerrainLogic.h
#    Include/Win32Device/Common/Win32BIGFile.h
#    Include/Win32Device/Common/Win32BIGFileSystem.h
    Include/Win32Device/Common/Win32CDManager.h
    Include/Win32Device/Common/Win32GameEngine.h
#    Include/Win32Device/Common/Win32LocalFile.h
#    Include/Win32Device/Common/Win32LocalFileSystem.h
    Include/Win32Device/GameClient/Win32DIKeyboard.h
    Include/Win32Device/GameClient/Win32Mouse.h
#    Source/MilesAudioDevice/MilesAudioManager.cpp
    Source/VideoDevice/Bink/BinkVideoPlayer.cpp
    Source/W3DDevice/Common/System/W3DFunctionLexicon.cpp
    Source/W3DDevice/Common/System/W3DRadar.cpp
    Source/W3DDevice/Common/Thing/W3DModuleFactory.cpp
    Source/W3DDevice/Common/Thing/W3DThingFactory.cpp
    Source/W3DDevice/Common/W3DConvert.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DDebrisDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DDefaultDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DDependencyModelDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DLaserDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DModelDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DOverlordTankDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DPoliceCarDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DProjectileStreamDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DRopeDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DScienceModelDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DSupplyDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DTankDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DTankTruckDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DTracerDraw.cpp
    Source/W3DDevice/GameClient/Drawable/Draw/W3DTruckDraw.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DCheckBox.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DComboBox.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DHorizontalSlider.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DListBox.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DProgressBar.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DPushButton.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DRadioButton.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DStaticText.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DTabControl.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DTextEntry.cpp
    Source/W3DDevice/GameClient/GUI/Gadget/W3DVerticalSlider.cpp
    Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DControlBar.cpp
    Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DMainMenu.cpp
    Source/W3DDevice/GameClient/GUI/GUICallbacks/W3DMOTD.cpp
    Source/W3DDevice/GameClient/GUI/W3DGameFont.cpp
    Source/W3DDevice/GameClient/GUI/W3DGameWindow.cpp
    Source/W3DDevice/GameClient/GUI/W3DGameWindowManager.cpp
    Source/W3DDevice/GameClient/HeightMap.cpp
    Source/W3DDevice/GameClient/Shaders/invmonochrome.nvp
    Source/W3DDevice/GameClient/Shaders/monochrome.nvp
    Source/W3DDevice/GameClient/Shaders/motionblur.nvp
    Source/W3DDevice/GameClient/Shaders/MotionBlur.nvv
    Source/W3DDevice/GameClient/Shaders/roadnoise2.nvp
    Source/W3DDevice/GameClient/Shaders/terrain.nvp
    Source/W3DDevice/GameClient/Shaders/terrainnoise.nvp
    Source/W3DDevice/GameClient/Shaders/terrainnoise2.nvp
    Source/W3DDevice/GameClient/Shadow/W3DBufferManager.cpp
    Source/W3DDevice/GameClient/Shadow/W3DProjectedShadow.cpp
    Source/W3DDevice/GameClient/Shadow/W3DShadow.cpp
    Source/W3DDevice/GameClient/Shadow/W3DVolumetricShadow.cpp
    Source/W3DDevice/GameClient/TerrainTex.cpp
    Source/W3DDevice/GameClient/TileData.cpp
    Source/W3DDevice/GameClient/W3DAssetManager.cpp
    Source/W3DDevice/GameClient/W3DAssetManagerExposed.cpp
    Source/W3DDevice/GameClient/W3DBibBuffer.cpp
    Source/W3DDevice/GameClient/W3DBridgeBuffer.cpp
    Source/W3DDevice/GameClient/W3DCustomEdging.cpp
    Source/W3DDevice/GameClient/W3DDebugDisplay.cpp
    Source/W3DDevice/GameClient/W3DDebugIcons.cpp
    Source/W3DDevice/GameClient/W3DDisplay.cpp
    Source/W3DDevice/GameClient/W3DDisplayString.cpp
    Source/W3DDevice/GameClient/W3DDisplayStringManager.cpp
    Source/W3DDevice/GameClient/W3DDynamicLight.cpp
    Source/W3DDevice/GameClient/W3DFileSystem.cpp
    Source/W3DDevice/GameClient/W3DGameClient.cpp
    Source/W3DDevice/GameClient/W3DInGameUI.cpp
    Source/W3DDevice/GameClient/W3DMouse.cpp
    Source/W3DDevice/GameClient/W3DParticleSys.cpp
    Source/W3DDevice/GameClient/W3DPoly.cpp
    Source/W3DDevice/GameClient/W3DRoadBuffer.cpp
    Source/W3DDevice/GameClient/W3DScene.cpp
    Source/W3DDevice/GameClient/W3DShaderManager.cpp
    Source/W3DDevice/GameClient/W3DShroud.cpp
    Source/W3DDevice/GameClient/W3DStatusCircle.cpp
    Source/W3DDevice/GameClient/W3DTerrainTracks.cpp
    Source/W3DDevice/GameClient/W3DTerrainVisual.cpp
    Source/W3DDevice/GameClient/W3DTreeBuffer.cpp
    Source/W3DDevice/GameClient/W3DVideoBuffer.cpp
    Source/W3DDevice/GameClient/W3DView.cpp
    Source/W3DDevice/GameClient/W3dWaypointBuffer.cpp
    Source/W3DDevice/GameClient/W3DWebBrowser.cpp
    Source/W3DDevice/GameClient/Water/W3DWater.cpp
    Source/W3DDevice/GameClient/Water/W3DWaterTracks.cpp
    Source/W3DDevice/GameClient/Water/wave.nvp
    Source/W3DDevice/GameClient/Water/wave.nvv
    Source/W3DDevice/GameClient/WorldHeightMap.cpp
    Source/W3DDevice/GameLogic/W3DGameLogic.cpp
    Source/W3DDevice/GameLogic/W3DGhostObject.cpp
    Source/W3DDevice/GameLogic/W3DTerrainLogic.cpp
#    Source/Win32Device/Common/Win32BIGFile.cpp
#    Source/Win32Device/Common/Win32BIGFileSystem.cpp
    Source/Win32Device/Common/Win32CDManager.cpp
    Source/Win32Device/Common/Win32GameEngine.cpp
#    Source/Win32Device/Common/Win32LocalFile.cpp
#    Source/Win32Device/Common/Win32LocalFileSystem.cpp
    Source/Win32Device/Common/Win32OSDisplay.cpp
    Source/Win32Device/GameClient/Win32DIKeyboard.cpp
    Source/Win32Device/GameClient/Win32Mouse.cpp
)

add_library(g_gameenginedevice STATIC)
set_target_properties(g_gameenginedevice PROPERTIES OUTPUT_NAME gameenginedevice)

target_sources(g_gameenginedevice PRIVATE ${GAMEENGINEDEVICE_SRC})

target_include_directories(g_gameenginedevice PUBLIC
    Include
)

target_link_libraries(g_gameenginedevice PRIVATE
    corei_gameenginedevice_private
    gi_always    
    gi_main  
)

target_link_libraries(g_gameenginedevice PUBLIC
    corei_gameenginedevice_public
    g_gameengine
)
