package de.vonaffenfels.Mobile;

import android.app.Activity;

import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;

import android.content.Context;

public class FirebaseHelper
{
    private Activity m_qtActivity;

    FirebaseHelper(Activity qtActivity)
    {
        m_qtActivity = qtActivity;
    }

    public String getData()
    {
        System.out.println("public static String getData()");
        return "['hallo','FirebaseHelper']";
    }
}
