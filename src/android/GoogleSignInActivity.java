package com.blackgrain.android.googleauth;

import org.qtproject.qt5.android.bindings.QtActivity;

import android.app.Activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.AuthCredential;
import com.google.firebase.auth.GetTokenResult;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.auth.GoogleAuthProvider;

public class GoogleSignInActivity {

    private static final String TAG = "GoogleSignInActivity";

    // RC_SIGN_IN 987654321 this is a random number i picked. if you change it
    // do not forget to change it in your BaseActivity
    private static final int RC_SIGN_IN = 987654321;

    long m_handler;
    Activity m_qtActivity;
    String m_clientId;

    public FirebaseUser m_user;
    public FirebaseAuth m_auth;
    public GoogleSignInClient m_googleSignInClient;

    public GoogleSignInActivity(String clientId,long handler, Activity qtActivity)
    {
        m_handler = handler;
        m_qtActivity = qtActivity;
        m_clientId = clientId;
    }

    public void login()
    {
        // Configure Google Sign In
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
            .requestIdToken(m_clientId)
            .requestEmail()
            .build();

        m_googleSignInClient = GoogleSignIn.getClient(m_qtActivity.getApplicationContext(), gso);
        m_auth = FirebaseAuth.getInstance();

        // check if there is allready a user
        FirebaseUser currentUser = m_auth.getCurrentUser();

        if(currentUser != null){
            m_user = m_auth.getCurrentUser();
            getUserToken();
        } else {
            signIn();
        }
    }

    private void logout() {
        // Firebase sign out
        m_auth.signOut();

        // Google sign out
        m_googleSignInClient.signOut().addOnCompleteListener(m_qtActivity, new OnCompleteListener<Void>() {
            @Override
            public void onComplete(@NonNull Task<Void> task) {
                signedOut();
            }
        });
    }

    private void signIn() {
        Intent signInIntent = m_googleSignInClient.getSignInIntent();
        m_qtActivity.startActivityForResult(signInIntent, RC_SIGN_IN);
    }

    public void activityResult(Intent data) {
        Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try {
            // Google Sign In was successful, authenticate with Firebase
            GoogleSignInAccount account = task.getResult(ApiException.class);
            firebaseAuthWithGoogle(account);
        } catch (ApiException e) {
            // Google Sign In failed
            Log.w(TAG, "Google sign in failed", e);
        }
    }

    private void firebaseAuthWithGoogle(GoogleSignInAccount acct) {
        AuthCredential credential = GoogleAuthProvider.getCredential(acct.getIdToken(), null);
        m_auth.signInWithCredential(credential).addOnCompleteListener(m_qtActivity, new OnCompleteListener<AuthResult>() {

            @Override
            public void onComplete(@NonNull Task<AuthResult> task) {
                if (task.isSuccessful()) {
                    // Sign in success, update UI with the signed-in user's information
                    m_user = m_auth.getCurrentUser();
                    getUserToken();
                } else {
                    // sign in failed
                    Log.w(TAG, "signInWithCredential:failure", task.getException());
                }
            }
        });
    }

    private void getUserToken() {
        m_auth.getCurrentUser().getToken(true).addOnCompleteListener(m_qtActivity, new OnCompleteListener<GetTokenResult>() {
            @Override
            public void onComplete(@NonNull Task<GetTokenResult> task) {
                Log.d(TAG, "getUserToken" + task.isSuccessful());
                if (task.isSuccessful() == true) {
                    // todo check if task is isSuccessful?
                    // task.isSuccessful()
                    // GetTokenResult success
                    Log.d(TAG, "GetTokenResult success");
                    String token = task.getResult().getToken();
                    setCurrentUser(token);
                } else {
                    Log.d(TAG, "GetTokenResult failed");
                }
            }
        });

    }

    public static native void setCurrentUser(String userToken);
    public static native void signedOut();
}
