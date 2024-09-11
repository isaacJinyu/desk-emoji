import sounddevice as sd

from common import *


def callback(indata, frames, time, status):
    global audio_buffer, silence_duration, is_capturing

    if status: print(status)
    audio_buffer = np.concatenate([audio_buffer, indata[:, 0]])
    volume_norm = np.linalg.norm(indata) / np.sqrt(len(indata))

    if volume_norm < silence_threshold:
        silence_duration += block_duration
        if silence_duration >= silence_duration_threshold and is_capturing:
            is_capturing = False
            print("Silence detected. Capture stopped.")
            segments, info = model.transcribe(audio_buffer, beam_size=5, language='en')
            audio_buffer = np.zeros((0,), dtype=np.float32)
            print("Detected language '%s' with probability %f" % (info.language, info.language_probability))
            for segment in segments:
                question = segment.text.strip()
                if not question: continue
                print(f"You: {question}")
                answer = get_completion(segment.text, temperature=1)
                print(f"Bot: {answer}\n")
                speak_text(answer)
                break

    else:
        if not is_capturing:
            print("Sound detected. Starting capture.")
        silence_duration = 0.0
        is_capturing = True


if __name__ == '__main__':
    with sd.InputStream(callback=callback, channels=1, samplerate=samplerate, dtype=np.float32):
        try:
            while True:
                sd.sleep(1000)
        except KeyboardInterrupt:
            print("Stopped.")
