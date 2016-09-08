/*
 * Copyright (C) 2008-2009 The Android Open Source Project
 * Copyright 2016 Google, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.languageresources.mymr;

import android.app.Dialog;
import android.inputmethodservice.InputMethodService;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.os.IBinder;
import android.text.InputType;
import android.text.method.MetaKeyKeyListener;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.inputmethod.CompletionInfo;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import com.google.languageresources.my.GraphemeComposer;
import com.google.languageresources.my.ZawgyiToUnicodeConverter;

/**
 * Example of a soft keyboard for entering text in Myanmar script.  This code is
 * focused on simplicity over completeness, so it should in no way be considered
 * to be a complete implementation.
 *
 * Based on
 *
 *   development/samples/SoftKeyboard/src/com/example/android/softkeyboard/SoftKeyboard.java
 *
 * available from
 *
 *   https://android.googlesource.com/platform/development/+/master/samples/SoftKeyboard/
 *
 */
public class MyanmarUnicodeKeyboard extends InputMethodService
        implements KeyboardView.OnKeyboardActionListener {
    static final boolean DEBUG = false;

    /**
     * This boolean indicates the optional example code for performing
     * processing of hard keys in addition to regular text generation
     * from on-screen interaction.  It would be used for input methods that
     * perform language translations (such as converting text entered on
     * a QWERTY keyboard to Chinese), but may not be used for input methods
     * that are primarily intended to be used for on-screen text entry.
     */
    static final boolean PROCESS_HARD_KEYS = true;

    static final int KEYCODE_PASTE_FROM_ZAWGYI = -100;
    static final int KEYCODE_LANGUAGE_SWITCH = -101;

    private InputMethodManager mInputMethodManager;

    private KeyboardView mInputView;
    private CandidateView mCandidateView;
    private CompletionInfo[] mCompletions;

    private GraphemeComposer mComposer = new GraphemeComposer("\u1031");
    private StringBuilder mComposed = new StringBuilder(8);

    private boolean mPredictionOn;
    private boolean mCompletionOn;
    private int mLastDisplayWidth;
    private long mMetaState;

    private Keyboard mBurmeseKeyboard;
    private Keyboard mBurmeseShiftedKeyboard;

    private static final String SEPARATORS = " \n\u104A\u104B";

    private static final ZawgyiToUnicodeConverter ZAWGYI_TO_UNICODE_CONVERTER =
            new ZawgyiToUnicodeConverter();

    /**
     * Main initialization of the input method component.  Be sure to call
     * to super class.
     */
    @Override
    public void onCreate() {
        super.onCreate();
        mInputMethodManager = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
    }

    /**
     * This is the point where you can do all of your UI initialization.  It
     * is called after creation and any configuration change.
     */
    @Override
    public void onInitializeInterface() {
        if (mBurmeseKeyboard != null) {
            // Configuration changes can happen after the keyboard gets recreated,
            // so we need to be able to re-build the keyboards if the available
            // space has changed.
            int displayWidth = getMaxWidth();
            if (displayWidth == mLastDisplayWidth) {
                return;
            }
            mLastDisplayWidth = displayWidth;
        }
        mBurmeseKeyboard = new Keyboard(this, R.xml.burmese_keyboard);
        mBurmeseShiftedKeyboard = new Keyboard(this, R.xml.burmese_shifted_keyboard);
    }

    /**
     * Called by the framework when your view for creating input needs to
     * be generated.  This will be called the first time your input method
     * is displayed, and every time it needs to be re-created such as due to
     * a configuration change.
     */
    @Override
    public View onCreateInputView() {
        mInputView = (KeyboardView) getLayoutInflater().inflate(R.layout.input, null);
        mInputView.setOnKeyboardActionListener(this);
        mInputView.setPreviewEnabled(false);
        setShiftedKeyboard(false);
        return mInputView;
    }

    private void setShiftedKeyboard(boolean shifted) {
        mInputView.setKeyboard(shifted ? mBurmeseShiftedKeyboard : mBurmeseKeyboard);
        mInputView.setShifted(shifted);
    }

    /**
     * Called by the framework when your view for showing candidates needs to
     * be generated, like {@link #onCreateInputView}.
     */
    @Override
    public View onCreateCandidatesView() {
        mCandidateView = new CandidateView(this);
        mCandidateView.setService(this);
        return mCandidateView;
    }

    /**
     * This is the main point where we do our initialization of the input method
     * to begin operating on an application.  At this point we have been
     * bound to the client, and are now receiving all of the detailed information
     * about the target of our edits.
     */
    @Override
    public void onStartInput(EditorInfo attribute, boolean restarting) {
        super.onStartInput(attribute, restarting);

        // Reset our state.  We want to do this even if restarting, because
        // the underlying state of the text editor could have changed in any way.
        mComposer.clear();
        updateCandidates();

        if (!restarting) {
            // Clear shift states.
            mMetaState = 0;
        }

        mPredictionOn = true;
        mCompletionOn = false;
        mCompletions = null;

        // We are now going to initialize our state based on the type of
        // text being edited.
        switch (attribute.inputType & InputType.TYPE_MASK_CLASS) {
            case InputType.TYPE_CLASS_TEXT:
                // We now look for a few special variations of text that will
                // modify our behavior.
                int variation = attribute.inputType & InputType.TYPE_MASK_VARIATION;
                if (variation == InputType.TYPE_TEXT_VARIATION_PASSWORD
                        || variation == InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD) {
                    // Do not display predictions / what the user is typing
                    // when they are entering a password.
                    mPredictionOn = false;
                }

                if ((attribute.inputType & InputType.TYPE_TEXT_FLAG_AUTO_COMPLETE) != 0) {
                    // If this is an auto-complete text view, then our predictions
                    // will not be shown and instead we will allow the editor
                    // to supply their own.  We only show the editor's
                    // candidates when in fullscreen mode, otherwise relying
                    // own it displaying its own UI.
                    mPredictionOn = false;
                    mCompletionOn = isFullscreenMode();
                }
                break;

            default:
                break;
        }
    }

    /**
     * This is called when the user is done editing a field.  We can use
     * this to reset our state.
     */
    @Override
    public void onFinishInput() {
        super.onFinishInput();

        // Clear current composing text and candidates.
        mComposer.clear();
        updateCandidates();

        // We only hide the candidates window when finishing input on
        // a particular editor, to avoid popping the underlying application
        // up and down if the user is entering text into the bottom of
        // its window.
        setCandidatesViewShown(false);

        if (mInputView != null) {
            mInputView.closing();
        }
    }

    @Override
    public void onStartInputView(EditorInfo attribute, boolean restarting) {
        super.onStartInputView(attribute, restarting);
        setShiftedKeyboard(false);
        mInputView.closing();
    }

    /**
     * Deal with the editor reporting movement of its cursor.
     */
    @Override
    public void onUpdateSelection(
            int oldSelStart,
            int oldSelEnd,
            int newSelStart,
            int newSelEnd,
            int candidatesStart,
            int candidatesEnd) {
        super.onUpdateSelection(
                oldSelStart, oldSelEnd, newSelStart, newSelEnd, candidatesStart, candidatesEnd);

        // If the current selection in the text view changes, we should
        // clear whatever candidate text we have.
        if (!mComposer.isEmpty() && (newSelStart != candidatesEnd || newSelEnd != candidatesEnd)) {
            mComposer.clear();
            updateCandidates();
            InputConnection ic = getCurrentInputConnection();
            if (ic != null) {
                ic.finishComposingText();
            }
        }
    }

    /**
     * This tells us about completions that the editor has determined based
     * on the current text in it.  We want to use this in fullscreen mode
     * to show the completions ourself, since the editor can not be seen
     * in that situation.
     */
    @Override
    public void onDisplayCompletions(CompletionInfo[] completions) {
        if (mCompletionOn) {
            mCompletions = completions;
            if (completions == null) {
                setSuggestions(null, false, false);
                return;
            }

            List<String> stringList = new ArrayList<String>();
            for (int i = 0; i < completions.length; i++) {
                CompletionInfo ci = completions[i];
                if (ci != null) stringList.add(ci.getText().toString());
            }
            setSuggestions(stringList, true, true);
        }
    }

    /**
     * This translates incoming hard key events into edit operations on an
     * InputConnection.  It is only needed when using the
     * PROCESS_HARD_KEYS option.
     */
    private boolean translateKeyDown(int keyCode, KeyEvent event) {
        mMetaState = MetaKeyKeyListener.handleKeyDown(mMetaState, keyCode, event);
        int c = event.getUnicodeChar(MetaKeyKeyListener.getMetaState(mMetaState));
        mMetaState = MetaKeyKeyListener.adjustMetaAfterKeypress(mMetaState);
        InputConnection ic = getCurrentInputConnection();
        if (c == 0 || ic == null) {
            return false;
        }

        onKey(c, null);

        return true;
    }

    /**
     * Use this to monitor key events being delivered to the application.
     * We get first crack at them, and can either resume them or let them
     * continue to the app.
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
                // The InputMethodService already takes care of the back
                // key for us, to dismiss the input method if it is shown.
                // However, our keyboard could be showing a pop-up window
                // that back should dismiss, so we first allow it to do that.
                if (event.getRepeatCount() == 0 && mInputView != null) {
                    if (mInputView.handleBack()) {
                        return true;
                    }
                }
                break;

            case KeyEvent.KEYCODE_DEL:
                // Special handling of the delete key: if we currently are
                // composing text for the user, we want to modify that instead
                // of letting the application do the delete itself.
                if (!mComposer.isEmpty()) {
                    onKey(Keyboard.KEYCODE_DELETE, null);
                    return true;
                }
                break;

            case KeyEvent.KEYCODE_ENTER:
                commitTyped(getCurrentInputConnection());
                // Let the underlying text editor always handle these.
                return false;

            default:
                // For all other keys, if we want to do transformations on
                // text being entered with a hard keyboard, we need to process
                // it and do the appropriate action.
                if (PROCESS_HARD_KEYS) {
                    if (translateKeyDown(keyCode, event)) {
                        return true;
                    }
                }
        }

        return super.onKeyDown(keyCode, event);
    }

    /**
     * Use this to monitor key events being delivered to the application.
     * We get first crack at them, and can either resume them or let them
     * continue to the app.
     */
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // If we want to do transformations on text being entered with a hard
        // keyboard, we need to process the up events to update the meta key
        // state we are tracking.
        if (PROCESS_HARD_KEYS) {
            mMetaState = MetaKeyKeyListener.handleKeyUp(mMetaState, keyCode, event);
        }

        return super.onKeyUp(keyCode, event);
    }

    /**
     * Helper function to commit any text being composed in to the editor.
     */
    private void commitTyped(InputConnection inputConnection) {
        if (!mComposer.isEmpty()) {
            mComposed.setLength(0);
            mComposer.flushTo(mComposed);
            inputConnection.commitText(mComposed, mComposed.length());
            updateCandidates();
        }
    }

    /**
     * Helper to send a key down / key up pair to the current editor.
     */
    private void keyDownUp(int keyEventCode) {
        getCurrentInputConnection().sendKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, keyEventCode));
        getCurrentInputConnection().sendKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, keyEventCode));
    }

    /**
     * Helper to send a character to the editor as raw key events.
     */
    private void sendKey(int keyCode) {
        switch (keyCode) {
            case '\n':
                keyDownUp(KeyEvent.KEYCODE_ENTER);
                break;
            default:
                if (keyCode >= '0' && keyCode <= '9') {
                    keyDownUp(keyCode - '0' + KeyEvent.KEYCODE_0);
                } else {
                    getCurrentInputConnection().commitText(String.valueOf((char) keyCode), 1);
                }
                break;
        }
    }

    // Implementation of KeyboardViewListener

    @Override
    public void onKey(int primaryCode, int[] keyCodes) {
        switch (primaryCode) {
            case Keyboard.KEYCODE_DELETE:
                handleBackspace();
                break;
            case Keyboard.KEYCODE_SHIFT:
                handleShift();
                break;
            case Keyboard.KEYCODE_CANCEL:
                handleClose();
                break;
            case KEYCODE_PASTE_FROM_ZAWGYI:
                handlePasteFromZawgyi();
                break;
            case KEYCODE_LANGUAGE_SWITCH:
                handleLanguageSwitch();
                break;
            default:
                if (isSeparator(primaryCode)) {
                    // Handle separator
                    commitTyped(getCurrentInputConnection());
                    sendKey(primaryCode);
                    setShiftedKeyboard(false);
                } else {
                    handleCharacter(primaryCode, keyCodes);
                }
                break;
        }
    }

    @Override
    public void onText(CharSequence text) {
        InputConnection ic = getCurrentInputConnection();
        if (ic == null) {
            return;
        }
        ic.beginBatchEdit();
        commitTyped(ic);
        ic.commitText(text, 0);
        ic.endBatchEdit();
    }

    /**
     * Update the list of available candidates from the current composing
     * text.  This will need to be filled in by however you are determining
     * candidates.
     */
    private void updateCandidates() {
        if (!mCompletionOn) {
            if (mPredictionOn && !mComposer.isEmpty()) {
                ArrayList<String> list = new ArrayList<String>();
                list.add(mComposer.toString());
                setSuggestions(list, true, true);
            } else {
                setSuggestions(null, false, false);
            }
        }
    }

    public void setSuggestions(
            List<String> suggestions, boolean completions, boolean typedWordValid) {
        if (suggestions != null && suggestions.size() > 0) {
            setCandidatesViewShown(true);
        } else if (isExtractViewShown()) {
            setCandidatesViewShown(true);
        }
        if (mCandidateView != null) {
            mCandidateView.setSuggestions(suggestions, completions, typedWordValid);
        }
    }

    private void handleBackspace() {
        if (!mComposer.isEmpty()) {
            mComposer.remove();
            if (!mComposer.isEmpty()) {
                getCurrentInputConnection().setComposingText(mComposer.toString(), 1);
            } else {
                getCurrentInputConnection().commitText("", 0);
            }
            updateCandidates();
        } else {
            keyDownUp(KeyEvent.KEYCODE_DEL);
        }
    }

    private void handleShift() {
        if (mInputView == null) {
            return;
        }

        Keyboard currentKeyboard = mInputView.getKeyboard();
        if (mBurmeseKeyboard == currentKeyboard) {
            setShiftedKeyboard(true);
        } else if (mBurmeseShiftedKeyboard == currentKeyboard) {
            setShiftedKeyboard(false);
        }
    }

    private void handleCharacter(int primaryCode, int[] keyCodes) {
        if (primaryCode <= 0) {
            return;
        }
        mComposed.setLength(0);
        InputConnection ic = getCurrentInputConnection();
        ic.beginBatchEdit();
        if ('\u1040' <= primaryCode && primaryCode <= '\u104F') {
            mComposer.flushTo(mComposed);
            mComposed.appendCodePoint(primaryCode);
            if (primaryCode == '\u104E') {
                mComposed.append("\u1004\u103A\u1038");
            }
            ic.commitText(mComposed, 1);
            mComposed.setLength(0);
        } else {
            if (primaryCode == '\u1031') {
                mComposer.flushTo(mComposed);
            }
            mComposer.add(primaryCode, mComposed);
            if (mComposed.length() > 0) {
                ic.commitText(mComposed, 1);
            }
            ic.setComposingText(mComposer.toString(), 1);
        }
        ic.endBatchEdit();
        updateCandidates();
    }

    private void handleClose() {
        commitTyped(getCurrentInputConnection());
        requestHideSelf(0);
        mInputView.closing();
    }

    private CharSequence getTextFromClipboard() {
        android.text.ClipboardManager cm =
                (android.text.ClipboardManager)
                        getSystemService(android.content.Context.CLIPBOARD_SERVICE);
        return cm.getText();
    }

    // In API level 11 and above:
    //
    // private CharSequence getTextFromClipboard() {
    //     android.content.ClipboardManager cm =
    //             (android.content.ClipboardManager)
    //                     getSystemService(android.content.Context.CLIPBOARD_SERVICE);
    //     android.content.ClipData clip = cm.getPrimaryClip();
    //     if (clip == null) {
    //         return "";
    //     }
    //     android.content.ClipData.Item item = clip.getItemAt(0);
    //     return item.coerceToText(this).toString();
    // }

    private void handlePasteFromZawgyi() {
        CharSequence text = getTextFromClipboard();
        if (text.length() == 0) {
            return;
        }
        InputConnection ic = getCurrentInputConnection();
        if (ic == null) {
            return;
        }
        ic.commitText(ZAWGYI_TO_UNICODE_CONVERTER.convert(text), 1);
    }

    private IBinder getToken() {
        final Dialog dialog = getWindow();
        if (dialog == null) {
            return null;
        }
        final Window window = dialog.getWindow();
        if (window == null) {
            return null;
        }
        return window.getAttributes().token;
    }

    private void handleLanguageSwitch() {
        try {
            Method m =
                    InputMethodManager.class
                            .getMethod(
                                    "switchToNextInputMethod",
                                    new Class[] {android.os.IBinder.class, boolean.class});
            m.invoke(mInputMethodManager, getToken(), false /* onlyCurrentIme */);
            return;
        } catch (NoSuchMethodException e) {
            // Android API level < 14. Do nothing.
        } catch (IllegalAccessException | InvocationTargetException e) {
            // This cannot happen.
            throw new AssertionError(e);
        }
        try {
            Method m =
                    InputMethodManager.class
                            .getMethod(
                                    "switchToLastInputMethod",
                                    new Class[] {android.os.IBinder.class});
            m.invoke(mInputMethodManager, getToken());
            return;
        } catch (NoSuchMethodException e) {
            // Android API level < 11. Do nothing.
        } catch (IllegalAccessException | InvocationTargetException e) {
            // This cannot happen.
            throw new AssertionError(e);
        }
    }

    public boolean isSeparator(int code) {
        return SEPARATORS.contains(String.valueOf((char) code));
    }

    public void pickDefaultCandidate() {
        pickSuggestionManually(0);
    }

    public void pickSuggestionManually(int index) {
        if (mCompletionOn && mCompletions != null && index >= 0 && index < mCompletions.length) {
            CompletionInfo ci = mCompletions[index];
            getCurrentInputConnection().commitCompletion(ci);
            if (mCandidateView != null) {
                mCandidateView.clear();
            }
        } else if (!mComposer.isEmpty()) {
            // If we were generating candidate suggestions for the current
            // text, we would commit one of them here.  But for this sample,
            // we will just commit the current text.
            commitTyped(getCurrentInputConnection());
        }
    }

    public void swipeRight() {
        if (mCompletionOn) {
            pickDefaultCandidate();
        }
    }

    public void swipeLeft() {
        handleBackspace();
    }

    public void swipeDown() {
        handleClose();
    }

    public void swipeUp() {}

    public void onPress(int primaryCode) {}

    public void onRelease(int primaryCode) {}
}
