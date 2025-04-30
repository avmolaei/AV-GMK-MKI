
# AV-GMK MK I

  

The AV-GMK MK I is the first iteration of the AVlabs AV-GMK, the AVlabs "normal Gaming Mechanical Keyboard.

It differs from previous AVlabs projects by the fact that it has a normal ISO AZERTY layout. However, the alpha keys are in a ortholinear layout.

Epic RGB lighting powered by SK6812-mini LEDs

Compatible with MX style switches.

  
  

## TODO: shit to do still:

  

FW: Most keyboard logic has been implemented, ported from AV-RK MK2. Notorious "cannot hold down key" bug of AV-RK MK2 has been fixed. LED modese have been implemented as follows: 



One mode should be off, duh. (5)

One mode should be green lighting (3)

One mode should be a lighting per zone (e.g one color for Fn keys, one color for alphanumeric, etc.). This is the default lighting mode upon power up (1 )
- NB: I am a bit lazy to implement a logic to modify on the fly the colors for each zone, so this will be hardcoded. A good improvement would be to do that on the fly and store the color scheme. 

One mode should be the rainbow wave.  (2)

One function should allow brightnes control (+ -)

One mode should be white. (4)



  
  

PCB: The "-" Key on the numpad is rotated 90° for some reason.

All stabilizer footprints are messed up, the radius of the hole for the clip is too small and the length between both mounting posts are fucked. Plastic post can be chopped down, and the other one can be screwed and it works normally, for peace of mind a slight point of glue can be added.

The stabilizer holes on the backspace key are way too small.

more LEDs should be added. while 1 LED/key is standard on most keyboard, it feels "empty" near big keys like the spacebar.

Add kailh choc footprints next to the MX holes for optimal switch compatibility

Implement the knob functionnality for volume controls. 

  

CAD: finish cad, duh.
