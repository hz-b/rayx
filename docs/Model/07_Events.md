# Events

When the tracing process is finished, our Tracer does not only return the final state of the rays;
additionally it provides us with a sequence of events - expressing what happened to each ray individually.
This allows us to fully understand what happened to each ray, and potentially where in the beamline it diverged from our expectations.

Each of these events contains a "snapshot" of the ray at that point in time,
and information about what happened at this point (encoded as an "EventType").

The Events are:

- Uninitialized: The ray was not jet initialized.
- Emitted: The ray just got emmited by the light source.
- HitElement: The ray did just intersect with an element.
- FatalError: An Error which is Fatal
- Absorbed: The ray was absorbed by the element.
- BeyondHorizon: The Ray did not hit any more elements and instead it will now fly in the same direction forever.
- TooManyEvents: The Event Limit is reached and no more Events for this ray got traced. 


Some EventTypes "finalize" the corresponding ray, preventing it from being processed further.
Examples for this are "Fly Off" and "Absorbed".



 

For more details see the Shared/EventType.h file.
