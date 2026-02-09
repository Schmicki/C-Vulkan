Get-ChildItem *.vert | ForEach-Object {
    &"$Env:VULKAN_SDK/Bin/glslc.exe" "$_" -o "../spv/$($_.Name).spv"
}

Get-ChildItem *.frag | ForEach-Object {
    &"$Env:VULKAN_SDK/Bin/glslc.exe" "$_" -o "../spv/$($_.Name).spv"
}