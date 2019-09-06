package com.mapbox.mapboxsdk.maps;

import android.support.v4.view.animation.PathInterpolatorCompat;
import android.view.animation.Interpolator;

import java.util.concurrent.CopyOnWriteArrayList;

public abstract class CameraTransition<T> {

  //todo camera - reason should be an intdef/enum/object so that devs can extend without clashing with default values
  public static final int REASON_ANY = 0;
  public static final int REASON_GESTURE = 1;
  public static final int REASON_LOCATION = 2;

  public static final Interpolator INTERPOLATOR_LINEAR = PathInterpolatorCompat.create(0f, 0f, 1f, 1f);
  public static final Interpolator INTERPOLATOR_EASING = PathInterpolatorCompat.create(0f, 0f, 0.25f, 1f);

  public static final int PROPERTY_CENTER = 0;
  public static final int PROPERTY_ZOOM = 1;
  public static final int PROPERTY_PITCH = 2;
  public static final int PROPERTY_BEARING = 3;
  public static final int PROPERTY_ANCHOR = 4;
  public static final int PROPERTY_PADDING = 5;

  private final CopyOnWriteArrayList<Listener> listeners = new CopyOnWriteArrayList<>();

  private final int reason;

  private T startValue;
  private final T endValue;

  private double startTimeMillis;
  private double startTimeNanos;
  private double endTimeMillis;
  private double endTimeNanos;
  private final double durationMillis;
  private final double durationNanos;
  private final double delayMillis;
  private final double delayNanos;

  private boolean isCanceled;
  private boolean isStarted;
  private boolean isFinished;
  private boolean isFinishing;

  private final Interpolator interpolator;

  CameraTransition(int reason, double durationMillis, double delayMillis, T endValue, Interpolator interpolator) {
    this.reason = reason;
    this.durationMillis = durationMillis;
    this.durationNanos = durationMillis * 1E6;
    this.delayMillis = delayMillis;
    this.delayNanos = this.delayMillis * 1E6;
    this.endValue = endValue;
    this.interpolator = interpolator;
  }

  void onStart(T startValue, double currentTimeNanos) {
    this.startTimeMillis = currentTimeNanos / 1E6 + delayMillis;
    this.startTimeNanos = currentTimeNanos + delayNanos;
    this.endTimeMillis = startTimeMillis + durationMillis;
    this.endTimeNanos = startTimeNanos + durationNanos;
    this.startValue = startValue;
    this.isStarted = true;
    this.isCanceled = false;
    this.isFinished = false;
  }

  public int getReason() {
    return reason;
  }

  public boolean isCanceled() {
    return isCanceled;
  }

  public void cancel() {
    isCanceled = true;
  }

  public boolean isStarted() {
    return isStarted;
  }

  public boolean isFinished() {
    return isFinished;
  }

  public T getStartValue() {
    if (!isStarted) {
      throw new RuntimeException("start or queue the transition first");
    }
    return startValue;
  }

  public T getEndValue() {
    return endValue;
  }

  public double getStartTime() {
    if (!isStarted) {
      throw new RuntimeException("start or queue the transition first");
    }
    return startTimeMillis;
  }

  double getStartTimeNanos() {
    if (!isStarted) {
      throw new RuntimeException("start or queue the transition first");
    }
    return startTimeNanos;
  }

  public double getEndTime() {
    if (!isStarted) {
      throw new RuntimeException("start or queue the transition first");
    }
    return endTimeMillis;
  }

  double getEndTimeNanos() {
    if (!isStarted) {
      throw new RuntimeException("start or queue the transition first");
    }
    return endTimeNanos;
  }

  public double getDuration() {
    return durationMillis;
  }

  public double getDelay() {
    return delayMillis;
  }

  public Interpolator getInterpolator() {
    return interpolator;
  }

  public void addListener(Listener listener) {
    listeners.add(listener);
  }

  public void removeListener(Listener listener) {
    listeners.remove(listener);
  }

  T onFrame(double currentTimeNanos) {
    double animationPosition = (currentTimeNanos - startTimeNanos) / durationNanos;
    double fraction = interpolator.getInterpolation((float) animationPosition);
    return getAnimatedValue(fraction);
  }

  boolean isFinishing() {
    return isFinishing;
  }

  void setFinishing() {
    isFinishing = true;
  }

  public void onProgress() {
    for (Listener listener : listeners) {
      listener.onProgress();
    }
  }

  void onCancel() {
    this.isCanceled = true;
    for (Listener listener : listeners) {
      listener.onCancel();
    }
    onFinish();
  }

  void onFinish() {
    this.startTimeMillis = 0;
    this.startTimeNanos = 0;
    this.endTimeMillis = 0;
    this.endTimeNanos = 0;
    this.startValue = null;
    this.isStarted = false;
    this.isFinished = true;
    for (Listener listener : listeners) {
      listener.onFinish();
    }
  }

  abstract int getCameraProperty();

  // todo camera - make protected?
  abstract T getAnimatedValue(double fraction);

  public interface Listener {

    void onProgress();

    void onCancel();

    void onFinish();
  }
}
