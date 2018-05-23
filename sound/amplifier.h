#ifndef SOUND_AMPLIFIER_H
#define SOUND_AMPLIFIER_H

#ifdef ENABLE_AUDIO

// Turns off amplifier when no audio is played.
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper, StateMachine, CommandParser {
public:
  Amplifier() : Looper(), CommandParser() {}
  const char* name() override { return "Amplifier"; }

  bool Active() {
//    if (saber_synth.on_) return true;
    if (audio_splicer.isPlaying()) return true;
    if (beeper.isPlaying()) return true;
    if (talkie.isPlaying()) return true;
    for (size_t i = 0; i < NELEM(wav_players); i++)
      if (wav_players[i].isPlaying())
        return true;
    return false;
  }

protected:
  void Setup() override {
    // Audio setup
    pinMode(amplifierPin, OUTPUT);
    SetupStandardAudio();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (Active()) YIELD();
      SLEEP(20);
      if (Active()) continue;
      STDOUT.println("Amplifier off.");
      digitalWrite(amplifierPin, LOW); // turn the amplifier off
      while (!Active()) YIELD();
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "amp")) {
      if (!strcmp(arg, "on")) {
        digitalWrite(amplifierPin, HIGH); // turn the amplifier off
        return true;
      }
      if (!strcmp(arg, "off")) {
        digitalWrite(amplifierPin, LOW); // turn the amplifier off
        return true;
      }
    }
    if (!strcmp(cmd, "whatison")) {
      bool on = false;
      SaberBase::DoIsOn(&on);
      STDOUT.print("Saber bases: ");
      STDOUT.println(on ? "On" : "Off");
      STDOUT.print("Audio splicer: ");
      STDOUT.println(audio_splicer.isPlaying() ? "On" : "Off");
      STDOUT.print("Beeper: ");
      STDOUT.println(beeper.isPlaying() ? "On" : "Off");
      STDOUT.print("Talker: ");
      STDOUT.println(talkie.isPlaying() ? "On" : "Off");
      for (size_t i = 0; i < NELEM(wav_players); i++) {
        STDOUT.print("Wav player ");
        STDOUT.print(i);
        STDOUT.print(": ");
        STDOUT.print(wav_players[i].isPlaying() ? "On" : "Off");
        STDOUT.print(" (eof =  ");
        STDOUT.print(wav_players[i].eof());
        STDOUT.print(" volume = ");
        STDOUT.print(wav_players[i].volume());
        STDOUT.println(")");
      }
      return true;
    }
    return false;
  }

  void Help() {
    STDOUT.println(" amp on/off - turn amplifier on or off");
  }
};

Amplifier amplifier;

void EnableAmplifier() {
  if (!digitalRead(amplifierPin)) {
    digitalWrite(amplifierPin, HIGH);
    delay(10); // Give it a little time to wake up.
  }
}

#endif   // enable audio
#endif
